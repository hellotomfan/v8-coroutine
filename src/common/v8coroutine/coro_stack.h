#pragma once


#include <stdlib.h>
# include <unistd.h>

#if CORO_USE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#if _POSIX_MAPPED_FILES
# include <sys/mman.h>
# define CORO_MMAP 1
# ifndef MAP_ANONYMOUS
#  ifdef MAP_ANON
#   define MAP_ANONYMOUS MAP_ANON
#  else
#   undef CORO_MMAP
#  endif
# endif
# include <limits.h>
#else
# undef CORO_MMAP
#endif

#if _POSIX_MEMORY_PROTECTION
# ifndef CORO_GUARDPAGES
#  define CORO_GUARDPAGES 4
# endif
#else
# undef CORO_GUARDPAGES
#endif

#if !CORO_MMAP
# undef CORO_GUARDPAGES
#endif

#if !__i386 && !__x86_64 && !__powerpc && !__m68k && !__alpha && !__mips && !__sparc64
# undef CORO_GUARDPAGES
#endif

#ifndef CORO_GUARDPAGES
# define CORO_GUARDPAGES 0
#endif

#if !PAGESIZE
#if !CORO_MMAP
#define PAGESIZE 4096
#else
	static size_t
coro_pagesize (void)
{
	static size_t pagesize;

	if (!pagesize)
		pagesize = sysconf (_SC_PAGESIZE);

	return pagesize;
}

#define PAGESIZE coro_pagesize ()
#endif
#endif


struct coro_stack
{
	  void *sptr;
	    size_t ssze;
#if CORO_USE_VALGRIND
		  int valgrind_id;
#endif
};


inline int
coro_stack_alloc (struct coro_stack *stack, unsigned int size)
{
	if (!size)
		size = 256 * 1024;

	stack->sptr = 0;
	stack->ssze = ((size_t)size * sizeof (void *) + PAGESIZE - 1) / PAGESIZE * PAGESIZE;

#if CORO_FIBER

	stack->sptr = (void *)stack;
	return 1;

#else

	size_t ssze = stack->ssze + CORO_GUARDPAGES * PAGESIZE;
	void *base;

#if CORO_MMAP
	/* mmap supposedly does allocate-on-write for us */
	base = mmap (0, ssze, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (base == (void *)-1)
	{
		/* some systems don't let us have executable heap */
		/* we assume they won't need executable stack in that case */
		base = mmap (0, ssze, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

		if (base == (void *)-1)
			return 0;
	}

#if CORO_GUARDPAGES
	mprotect (base, CORO_GUARDPAGES * PAGESIZE, PROT_NONE);
#endif

	base = (void*)((char *)base + CORO_GUARDPAGES * PAGESIZE);
#else
	base = malloc (ssze);
	if (!base)
		return 0;
#endif

#if CORO_USE_VALGRIND
	stack->valgrind_id = VALGRIND_STACK_REGISTER ((char *)base, ((char *)base) + ssze - CORO_GUARDPAGES * PAGESIZE);
#endif

	stack->sptr = base;
	return 1;

#endif
}


inline void
coro_stack_free (struct coro_stack *stack)
{
#if CORO_FIBER
  /* nop */
#else
  #if CORO_USE_VALGRIND
    VALGRIND_STACK_DEREGISTER (stack->valgrind_id);
  #endif

  #if CORO_MMAP
    if (stack->sptr)
      munmap ((void*)((char *)stack->sptr - CORO_GUARDPAGES * PAGESIZE),
              stack->ssze                 + CORO_GUARDPAGES * PAGESIZE);
  #else
    free (stack->sptr);
  #endif
#endif
}


