#include <assert.h>	
#include <string.h>	

//locais include
#include "mstring.h"

#define	INCREMENT_SIZE		1000000

static	char*	buf		= 0;
static	int	buf_size	= 0;	/* total size of buffer */
static	size_t	buf_avail	= 0;	/* free bytes in buf */
static	char*	next_name	= 0;	/* address available for next string */

#define	INVARIANT	assert(buf+buf_size==next_name+buf_avail); \
			assert(buf_size>=0); \
			assert(buf_avail>=0); \
			assert(next_name>=buf); 

static void grow(size_t size)
{
    size_t next_offset = next_name - buf;
    { INVARIANT }
    buf	= (char*)realloc(buf,buf_size+size); /* like malloc() if buf==0 */
    if (!buf) {
	fprintf(stderr,"Cannot expand name space (%d bytes)",buf_size+size);
	exit(1);
    }

  buf_avail	+= size;
  buf_size	+= size;
  next_name	= buf+next_offset;

  { INVARIANT }

}

char* str(char* s)
{
    char* ps;
    size_t l = strlen(s)+1; /* include trailing '\0' */
    
    { INVARIANT }
    while (l>buf_avail)
	grow(INCREMENT_SIZE);
    ps= strcpy(next_name,s);
    buf_avail-= l;
    next_name += l;
 
    { INVARIANT }

    return ps;
}






