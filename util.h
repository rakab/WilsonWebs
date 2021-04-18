/* See LICENSE file for copyright and license details. */

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))
#define NELEMS(A)               (sizeof(A) / sizeof((A)[0]))

void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);
