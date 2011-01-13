#ifndef RMD160_H
#define RMD160_H

#define RMD160_HASH_SIZE 20

void
rmd160_hash_buffer (char *outbuf, const char *buffer, size_t length);

#endif /*RMD160_H*/
