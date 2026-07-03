/* Minimal replacements for the few Intel C++ runtime helpers that the
   icl9-compiled dependency libs (curl/openssl/zlib) reference. Each just
   forwards to the standard CRT. Avoids needing libmmd/libirc/svml. */
#include <string.h>
#include <stddef.h>

/* linker symbol __intel_fast_memcpy  (C cdecl name _intel_fast_memcpy) */
void *_intel_fast_memcpy(void *d, const void *s, size_t n) { return memcpy(d, s, n); }

/* linker symbol __intel_fast_memset  (C cdecl name _intel_fast_memset) */
void *_intel_fast_memset(void *d, int c, size_t n) { return memset(d, c, n); }

/* linker symbol ___intel_sse2_strlen (C cdecl name __intel_sse2_strlen) */
size_t __intel_sse2_strlen(const char *s) { return strlen(s); }

/* linker symbol ___intel_VEC_memzero (C cdecl name __intel_VEC_memzero) */
void __intel_VEC_memzero(void *d, size_t n) { memset(d, 0, n); }
