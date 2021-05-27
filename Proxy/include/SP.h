#ifndef _SP_H_
#define _SP_H_

#ifdef __cplusplus
extern "C" {
#endif

bool SP_Start(const char *identity, const char *dn, int port);
void SP_Stop();


#ifdef __cplusplus
}
#endif
#endif
