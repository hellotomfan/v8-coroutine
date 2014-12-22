#include "v8_coroutine.h"
#include "coro_stack.h"
#include "libplatform/libplatform.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <cassert>
#include <ucontext.h>


static pthread_key_t floor_thread_key = 0;
static pthread_key_t ceil_thread_key = 0;
static pthread_key_t coro_thread_key = 0;

static void* find_thread_id_key(void* arg)
{
	v8::Isolate* isolate = static_cast<v8::Isolate*>(arg);
	assert(isolate != NULL);
	v8::Locker locker(isolate);
	isolate->Enter();
	floor_thread_key = 0x7777;
	for (pthread_key_t ii = coro_thread_key - 1; ii >= (coro_thread_key >= 20 ? coro_thread_key - 20 : 0); --ii) 
	{
		if (pthread_getspecific(ii) == isolate) 
		{
			floor_thread_key = ii;
			break;
		}
	}
	assert(floor_thread_key != 0x7777);
	ceil_thread_key = floor_thread_key + V8Coroutine::v8_tls_keys - 1;
	isolate->Exit();
	return NULL;
}

V8Coroutine::Coro::Coro(int stack_size, V8Coroutine *mgr_)
    : status(V8Coroutine::Coro_Ready)
    , back(0)
	, mgr(mgr_)
	, id(0)
    , initialized(false)
	, terminate(false)
{

	if (!coro_stack_alloc(&stack, stack_size)) {
		exit(1);
	}

	memset(v8_tls_data, 0, sizeof(v8_tls_data));
}

V8Coroutine::Coro::Coro(): status(V8Coroutine::Coro_Ready), back(0), id(0)
{
	stack.sptr = 0;
	stack.ssze = 0;
}


V8Coroutine::Coro::~Coro()
{
	if (stack.sptr) 
	{
		coro_stack_free(&stack);
	}
}

V8Coroutine::V8Coroutine()
{
    m_main_coro = new Coro;
    m_main_coro->status = V8Coroutine::Coro_Running;
    m_running_coro = m_main_coro;

	V8_Initialize();
}

V8Coroutine::~V8Coroutine()
{
	while (!m_pools.empty())
	{
		Coro *coro = m_pools.top();
		coro->Terminate();
		delete coro;

		m_pools.pop();
	}

    for(CORO_MAP::iterator i = m_coros.begin(); i != m_coros.end(); ++i)
    {
        delete i->second;
    }
    delete m_main_coro;

	V8_Shutdown();
}

void V8Coroutine::Coro::Execute()
{
	v8::Locker locker(mgr->GetIsolate());
	while (!terminate)
	{
		exec->Execute();
		status = V8Coroutine::Coro_Dead;
		{
			v8::Unlocker unlocker(mgr->GetIsolate());
			mgr->SwapContext(this, this->back);
		}
	}
}
void V8Coroutine::Coro::Trampoline(uint32_t low32, uint32_t hi32)
{
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    Coro* coro = (Coro*)ptr;
	coro->Execute();
	coro->mgr->SwapContext(coro, coro->back);
}

void V8Coroutine::Coro::Detach()
{
	delete exec;
	id = 0;
	yielded_returns = 0;
	exec = NULL;
}

void V8Coroutine::Coro::Resume(void *args)
{
	yielded_returns = args;

	back = mgr->m_running_coro;
	status = V8Coroutine::Coro_Running;

	mgr->m_running_coro->status = V8Coroutine::Coro_Normal;
	mgr->m_running_coro = this;

	mgr->SwapContext(back, this);

	mgr->m_running_coro = back;
	mgr->m_running_coro->status = V8Coroutine::Coro_Running;
}

void V8Coroutine::Coro::Initialize()
{
	getcontext(&context);
	context.uc_stack.ss_sp = stack.sptr;
	context.uc_stack.ss_size = stack.ssze;
	context.uc_link = NULL; 

	uintptr_t ptr = (uintptr_t)this;
	makecontext(&context, (void (*)(void))Trampoline, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));  

	initialized = true;
}

void V8Coroutine::Coro::Terminate()
{
	terminate = true;
	Resume(NULL);
}

void V8Coroutine::V8_Initialize() 
{
	platform = v8::platform::CreateDefaultPlatform(1);
	v8::V8::InitializePlatform(platform);
	v8::V8::Initialize();
	v8::Isolate::CreateParams params;
	params.constraints.ConfigureDefaults(2*1024*1024*1024llu, 0, 1);
	isolate = v8::Isolate::New(params);
	{
		v8::Locker locker(isolate);
		v8::Unlocker unlocker(isolate);
		pthread_key_create(&coro_thread_key, NULL);
		pthread_setspecific(coro_thread_key, NULL);
		pthread_t thread;
		pthread_create(&thread, NULL, find_thread_id_key, isolate);
		pthread_join(thread, NULL);
	}
}

void V8Coroutine::V8_Shutdown()
{
	isolate->Dispose();
	v8::V8::Dispose();
	v8::V8::ShutdownPlatform();
	delete platform;
}


V8Coroutine::Coro *V8Coroutine::Create(V8Coroutine::Executable *exec)
{
	Coro *coro = NULL;
	if (!m_pools.empty())
	{
		coro = m_pools.top();
		m_pools.pop();
		coro->status = V8Coroutine::Coro_Ready;
	}
	else
	{
		coro = new Coro(DEFAULT_CORO_STACKSIZE, this);
	}
	coro->id = ++id;
	coro->exec = exec;
	m_coros.insert(std::make_pair(coro->id, coro));
    return coro;
}

bool V8Coroutine::Resume(Coro * coro_, void *args)
{
    CORO_MAP::iterator iter = m_coros.find(coro_->id);
    if(iter == m_coros.end() && iter->second != coro_)
    {
        return false;
    }

    Coro* coro = coro_;
    switch(coro->status)
    {
		case V8Coroutine::Coro_Ready:
        {
			if (!coro->initialized)
			{
				coro->Initialize();
			}
            break;
        }

		case V8Coroutine::Coro_Suspended:
            break;

        default:
            return false;
    }

	coro->Resume(args);

    if(coro->status == V8Coroutine::Coro_Dead)
    {
		m_pools.push(coro);
		m_coros.erase(coro->id);
		coro->Detach();
    }
    return true;
}

void* V8Coroutine::Yield() 
{
    if(m_running_coro == m_main_coro)
    {
        return NULL;
    }

    Coro* coro = m_running_coro;
    coro->status = V8Coroutine::Coro_Suspended;

	{
		v8::Unlocker unlocker(isolate);
		SwapContext(coro, coro->back);
	}
    return coro->yielded_returns;
}

V8Coroutine::Coro * V8Coroutine::Running()
{
    return (m_running_coro == m_main_coro) ? 0 : m_running_coro; 
}

V8Coroutine::CoroStatus V8Coroutine::Status(Coro * coro)
{
    CORO_MAP::iterator it = m_coros.find(coro->id);
    if(it == m_coros.end())
    {
        return V8Coroutine::Coro_Dead; 
    }
    return coro->status;
}

void V8Coroutine::SwapContext(Coro* old_coro, Coro* new_coro)
{
	for (pthread_key_t ii = floor_thread_key; ii <= ceil_thread_key; ++ii) 
	{
		old_coro->v8_tls_data[ii - floor_thread_key] = pthread_getspecific(ii);
		pthread_setspecific(ii, new_coro->v8_tls_data[ii - floor_thread_key]);
	}
    swapcontext(&old_coro->context, &new_coro->context);
}

V8Coroutine* V8Coroutine::Instance()
{
	static V8Coroutine s_coroutine;
	return &s_coroutine;
}

V8Coroutine::Coro* V8Coroutine::Find(uint32 id)
{
	CORO_MAP::iterator iter = m_coros.find(id);
	if (iter != m_coros.end())
	{
		return iter->second;
	}
	return NULL;
}

