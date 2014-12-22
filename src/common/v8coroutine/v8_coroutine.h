#ifndef V8_COROUTINE_V8_COROUTINE_H
#define v8_COROUTINE_v8_COROUTINE_H


#include "v8.h"
#include "common.h"
#include "coro_stack.h"

#include <stdint.h>
#include <map>
#include <vector>
#include <stack>
#include <ucontext.h>

#define DEFAULT_CORO_STACKSIZE (1024 * 1024)

class V8Coroutine 
{
	friend class Coro;

	public:
		static const int v8_tls_keys = 3;

	public:
		class Executable
		{
			public:
				virtual ~Executable() {}
				virtual void Execute() = 0;
		};

	public:
		enum CoroStatus 
		{
			//coroutine is running
			Coro_Running,

			//coroutine is created but has not started
			Coro_Ready,

			//coroutine is yield
			Coro_Suspended,

			//coroutine is active but not running (that is ,it has resumed another coroutine)
			Coro_Normal,

			//coroutine has finished
			Coro_Dead,
		};


	public:
		class Coro
		{
			friend class V8Coroutine;

			private:
				Coro();
				Coro(int stack_size, V8Coroutine *mgr);
				~Coro();

			public:
				uint32 GetId() { return id; }

			private:
				void Execute();

			private:
				static void Trampoline(uint32_t low32, uint32_t hi32);

			private:
				void Detach();

			private:
				void Terminate();

			private:
				void Resume(void *args);
				void Initialize();

			private:
				V8Coroutine::CoroStatus status;
				ucontext_t context;
				Coro *back;
				coro_stack stack;
				uint32 id;
				V8Coroutine* mgr;
				bool terminate;

			private:
				V8Coroutine::Executable *exec;

			private:
				void *v8_tls_data[v8_tls_keys];
				void *yielded_returns;
				void *yielded_exception;

			private:
				bool initialized;
		};

	public:
		V8Coroutine();
		~V8Coroutine();

		//create a new coroutine with func
		Coro * Create(Executable *exec);

		//start or continue the execution of coroutine coro
		bool Resume(Coro * coro, void *args = NULL);

		//suspend the execution of calling coroutin
		void* Yield();

		//find coro by id
		Coro *Find(uint32 id);

		//return status of coro
		CoroStatus Status(Coro * coro);

		//return current running coroutin or 0 when called by main thread
		Coro * Running();

		//Get v8 isolate
		v8::Isolate *GetIsolate() { return isolate; }

		static V8Coroutine* Instance();

	private:
		void V8_Initialize();
		void V8_Shutdown();

	private:
		static void SwapContext(Coro* old_coro, Coro* new_coro);

	private:
		V8Coroutine(const V8Coroutine&);
		V8Coroutine& operator= (const V8Coroutine&);

	private:
		typedef std::map<uint32, Coro *> CORO_MAP;
		typedef std::stack<Coro *> CORO_STACK;

	private:
		CORO_MAP m_coros;
		CORO_STACK m_pools;

	private:
		Coro* m_running_coro;
		Coro* m_main_coro;

	private:
		v8::Isolate * isolate;
		v8::Platform* platform;	
		
	private:
		uint32 id;
};

#define sV8Coroutine V8Coroutine::Instance()

#endif
