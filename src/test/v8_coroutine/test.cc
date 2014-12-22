#include "v8_coroutine.h"


class V8Script: public V8Coroutine::Executable
{
	private:
		void Execute()
		{

			v8::Isolate *isolate = sV8Coroutine->GetIsolate();

			v8::Isolate::Scope isolate_scope(isolate);

			// Create a stack-allocated handle scope.
			v8::HandleScope handle_scope(isolate);

			// Create a new context.
			v8::Local<v8::Context> context = v8::Context::New(isolate);

			// Enter the context for compiling and running the hello world script.
			v8::Context::Scope context_scope(context);


			const char *s = 
				"arr = [];"
				"for (var i = 0; i < 1000; ++i) arr.push(1000-i);"
				"arr.sort(); arr[999];";

			// Create a string containing the JavaScript source code.
			//v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'");
			v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, s);

			// Compile the source code.
			v8::Local<v8::Script> script = v8::Script::Compile(source);


			int n = rand() % 100;
			for (int i = 0; i < n; ++i)
			{
				int *c = static_cast<int*>(sV8Coroutine->Yield());
				(*c)++;
			}

			// Run the script to get the result.
			v8::Local<v8::Value> result = script->Run();

			// Convert the result to an UTF8 string and print it.
			v8::String::Utf8Value utf8(result);

			printf("%s\n", *utf8);

		}
};

int main(int argc, char* argv[])
{
	std::vector<V8Coroutine::Coro*> v;
	for (int i = 0; i < 100; ++i)
	{
		v.push_back(sV8Coroutine->Create(new V8Script));
	}

	int c[100] = {0};

	while (!v.empty())
	{
		uint32 idx = rand() % v.size(); 
		sV8Coroutine->Resume(v[ idx ], &c[ v[ idx ]->GetId() - 1 ]);

		if (sV8Coroutine->Status(v[ idx ]) == V8Coroutine::Coro_Dead)
		{
			v.erase(v.begin() + idx);
		}
	}


	//for(int i = 0; i < 100; ++i)
	//{
	//	printf("%d\n", c[i]);
	//}	

}
