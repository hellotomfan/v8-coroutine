#include <v8.h>
#include <libplatform/libplatform.h>

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>

//#include <cctest.h>
//


#define CHECK(x) { printf("%d\n", x); }
#define CHECK_EQ(x, y) { printf("%d\n", x == y); }
#define CHECK_NE(x, y) { printf("%d\n", x != y); }


using namespace v8;


static inline v8::Local<v8::Value> v8_num(double x) {
	  return v8::Number::New(v8::Isolate::GetCurrent(), x);
}


static void DummyCallHandler(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//ApiTestFuzzer::Fuzz();
	args.GetReturnValue().Set(v8_num(13.4));
}

static void InstanceFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	//ApiTestFuzzer::Fuzz();
	args.GetReturnValue().Set(v8_num(12));
}

static void GlobalObjectInstancePropertiesGet( Local<String> key, const v8::PropertyCallbackInfo<v8::Value>&) {
	//  ApiTestFuzzer::Fuzz();
}






class LocalContext {
	public:
		LocalContext(v8::Isolate* isolate,
				v8::ExtensionConfiguration* extensions = 0,
				v8::Handle<v8::ObjectTemplate> global_template =
				v8::Handle<v8::ObjectTemplate>(),
				v8::Handle<v8::Value> global_object = v8::Handle<v8::Value>()) {
			Initialize(isolate, extensions, global_template, global_object);
		}

		LocalContext(v8::ExtensionConfiguration* extensions = 0,
				v8::Handle<v8::ObjectTemplate> global_template =
				v8::Handle<v8::ObjectTemplate>(),
				v8::Handle<v8::Value> global_object = v8::Handle<v8::Value>()) {
			Initialize(v8::Isolate::GetCurrent(), extensions, global_template, global_object);
		}

		virtual ~LocalContext() {
			v8::HandleScope scope(isolate_);
			v8::Local<v8::Context>::New(isolate_, context_)->Exit();
			context_.Reset();
			//printf("%s\n", __PRETTY_FUNCTION__);
		}

		v8::Context* operator->() {
			return *reinterpret_cast<v8::Context**>(&context_);
		}
		v8::Context* operator*() { return operator->(); }
		bool IsReady() { return !context_.IsEmpty(); }

		v8::Local<v8::Context> local() {
			return v8::Local<v8::Context>::New(isolate_, context_);
		}

	private:
		void Initialize(v8::Isolate* isolate,
				v8::ExtensionConfiguration* extensions,
				v8::Handle<v8::ObjectTemplate> global_template,
				v8::Handle<v8::Value> global_object) {
			v8::HandleScope scope(isolate);
			v8::Local<v8::Context> context = v8::Context::New(isolate,
					extensions,
					global_template,
					global_object);
			context_.Reset(isolate, context);
			context->Enter();

			isolate_ = isolate;
			//printf("%s\n", __PRETTY_FUNCTION__);
		}

		v8::Persistent<v8::Context> context_;
		v8::Isolate* isolate_;
};




//int main(int argc, char* argv[]) {
//
//	V8::Initialize();
//	for (int i = 0; i < 1; ++i)
//	{
//		// Create a new Isolate and make it the current one.
//		Isolate* isolate = Isolate::New();
//		{
//			Isolate::Scope isolate_scope(isolate);
//
//			// Create a stack-allocated handle scope.
//			HandleScope handle_scope(isolate);
//
//			Handle<Context> context = Context::New(isolate);
//			Handle<Context> context2 = Context::New(isolate); //
//			Persistent<Context> context3 = Context::New(isolate); 
//
//
//			printf("%d,%d,%d\n", sizeof(Isolate), sizeof(Local<Context>), sizeof(HandleScope));
//
//			{
//				// Enter the context for compiling and running the hello world script.
//				//Context::Scope context_scope(context);
//
//				context->Enter();
//
//				// Create a string containing the JavaScript source code.
//				Handle<String> source = String::NewFromUtf8(isolate, "'Hello' + ', World!'");
//				context->Exit();
//
//
//				context2->Enter();
//
//				Handle<String> source2 = String::NewFromUtf8(isolate, "'Hello' + ', World!222222'");
//				Handle<Script> script2 = Script::Compile(source2);
//
//				// Run the script to get the result.
//				Handle<Value> result2 = script2->Run();
//
//				// Convert the result to an UTF8 string and print it.
//				String::Utf8Value utf82(result2);
//				printf("%s\n", *utf82);
//
//
//
//
//
//
//				context2->Exit();
//
//
//				context->Enter();
//				// Compile the source code.
//				Handle<Script> script = Script::Compile(source);
//
//				// Run the script to get the result.
//				Handle<Value> result = script->Run();
//
//				// Convert the result to an UTF8 string and print it.
//				String::Utf8Value utf8(result);
//				printf("%s\n", *utf8);
//
//				context->Exit();
//			}
//		}
//		isolate->Dispose();
//
//	}
//	v8::V8::Dispose();
//
//	return 0;
//}


Handle<String> GetString(Isolate* isolate, const std::string& s)
{
	Handle<String> result = String::NewFromUtf8(isolate, s.c_str(), String::kNormalString, s.size());
	return result;
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) 
{
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Handle<Value> arg = args[0];
	String::Utf8Value value(arg);
	printf("%s\n", (*value));
}





/*
void test(Isolate* isolate, Persistent<Context>& persistent_context)
{
	Local<v8::Context> context = Local<v8::Context>::New(isolate, persistent_context);


	Context::Scope context_scope(context);


	// Create a string containing the JavaScript source code.
	//Handle<String> source = String::NewFromUtf8(isolate, "'Hello' + ', World!'");
	Handle<String> source = GetString(isolate, "log('hhhhh')");


	Handle<Script> script = Script::Compile(source);

	// Run the script to get the result.
	Handle<Value> result = script->Run();

	// Convert the result to an UTF8 string and print it.
	String::Utf8Value utf8(result);
	printf("%s\n", *utf8);
}
*/

void test(Isolate* isolate, Handle<Context>& context)
{
	//Local<v8::Context> context = Local<v8::Context>::New(isolate, persistent_context);


	Context::Scope context_scope(context);


	// Create a string containing the JavaScript source code.
	//Handle<String> source = String::NewFromUtf8(isolate, "'Hello' + ', World!'");
	Handle<String> source = GetString(isolate, "log('hhhhh')");


	Handle<Script> script = Script::Compile(source);

	// Run the script to get the result.
	Handle<Value> result = script->Run();

	// Convert the result to an UTF8 string and print it.
	String::Utf8Value utf8(result);
	printf("%s\n", *utf8);
}


/*
void SecurityChecks() {
  LocalContext env1;
  v8::HandleScope handle_scope(env1->GetIsolate());
  v8::Handle<Context> env2 = Context::New(env1->GetIsolate());

  Local<Value> foo = v8_str("foo");
  Local<Value> bar = v8_str("bar");

  // Set to the same domain.
  env1->SetSecurityToken(foo);

  // Create a function in env1.
  CompileRun("spy=function(){return spy;}");
  Local<Value> spy = env1->Global()->Get(v8_str("spy"));
  CHECK(spy->IsFunction());

  // Create another function accessing global objects.
  CompileRun("spy2=function(){return new this.Array();}");
  Local<Value> spy2 = env1->Global()->Get(v8_str("spy2"));
  CHECK(spy2->IsFunction());

  // Switch to env2 in the same domain and invoke spy on env2.
  {
    env2->SetSecurityToken(foo);
    // Enter env2
    Context::Scope scope_env2(env2);
    Local<Value> result = Function::Cast(*spy)->Call(env2->Global(), 0, NULL);
    CHECK(result->IsFunction());
  }

  {
    env2->SetSecurityToken(bar);
    Context::Scope scope_env2(env2);

    // Call cross_domain_call, it should throw an exception
    v8::TryCatch try_catch;
    Function::Cast(*spy2)->Call(env2->Global(), 0, NULL);
    CHECK(try_catch.HasCaught());
  }
}
*/



#include <v8.h>

using namespace v8;
static inline v8::Local<v8::String> v8_str(const char* x)
{
        return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), x);
}
static inline v8::Local<v8::Value> CompileRun(const char* source)
{
        return v8::Script::Compile(v8_str(source))->Run();
}


int main(int argc, char* argv[])
{
        v8::V8::Initialize();
        Isolate* isolate = Isolate::New();
        {
                Isolate::Scope isolate_scope(isolate);

		LocalContext env1;
		v8::HandleScope scope(isolate);

		// Create second environment.
		v8::Handle<Context> env2 = Context::New(env1->GetIsolate());

		Local<Value> foo = v8_str("foo");

		// Set same security token for env1 and env2.
		env1->SetSecurityToken(foo);
		env2->SetSecurityToken(foo);

		// Create a property on the global object in env2.
		{
			v8::Context::Scope scope(env2);
			env2->Global()->Set(v8_str("p"), v8::Integer::New(env2->GetIsolate(), 42));
		}

		// Create a reference to env2 global from env1 global.
		env1->Global()->Set(v8_str("other"), env2->Global());

		// Check that we have access to other.p in env2 from env1.
		Local<Value> result = CompileRun("other.p");
		CHECK(result->IsInt32());
		CHECK_EQ(42, result->Int32Value());

		printf("%s\n", *String::Utf8Value(env1->Global()->Get(v8_str("other"))));
		// Hold on to global from env2 and detach global from env2.
		Local<v8::Object> global2 = env2->Global();
		//env2->DetachGlobal();
		//env2->DetachGlobal();

		{
			v8::Context::Scope scope(env2);
			printf("%s\n", *String::Utf8Value(env2->Global()->Get(v8_str("p"))));
		}

		// Check that the global has been detached. No other.p property can
		// be found.
		//result = CompileRun("other.p");
		//CHECK(result.IsEmpty());

		// Reuse global2 for env3.
		v8::Handle<Context> env3 = Context::New(env1->GetIsolate(),
				0,
				v8::Handle<v8::ObjectTemplate>(),
				global2);
		//CHECK_EQ(global2, env3->Global());

		// Start by using the same security token for env3 as for env1 and env2.
		{
			v8::Context::Scope scope(env2);
			printf("%s\n", *String::Utf8Value(env2->Global()->Get(v8_str("p"))));
		}
		env3->SetSecurityToken(foo);

		// Create a property on the global object in env3.
		{
			v8::Context::Scope scope(env3);
			env3->Global()->Set(v8_str("p"), v8::Integer::New(env3->GetIsolate(), 24));
		}
		{
			v8::Context::Scope scope(env2);
			printf("%s\n", *String::Utf8Value(env2->Global()->Get(v8_str("p"))));
		}

		// Check that other.p is now the property in env3 and that we have access.
		result = CompileRun("other.p");
		CHECK(result->IsInt32());
		CHECK_EQ(24, result->Int32Value());

		result = CompileRun("other.p = 100");
		CHECK(result->IsInt32());
		CHECK_EQ(100, result->Int32Value());

		{
			v8::Context::Scope scope(env3);
			printf("%s\n", *String::Utf8Value(env3->Global()->Get(v8_str("p"))));
		}
		{
			v8::Context::Scope scope(env2);
			printf("%s\n", *String::Utf8Value(env2->Global()->Get(v8_str("p"))));

			v8::Local<v8::Context> t = Isolate::GetCurrentContext();
		}


		// Change security token for env3 to something different from env1 and env2.
		env3->SetSecurityToken(v8_str("bar"));

		// Check that we do not have access to other.p in env1. |other| is now
		// the global object for env3 which has a different security token,
		// so access should be blocked.
		//result = CompileRun("other.p");
		//CHECK(result.IsEmpty());



        }
        isolate->Dispose();
        v8::V8::Dispose();
        return 0;
}


