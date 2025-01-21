//
// Created by lukas on 18.02.2021.
//

#ifndef COND_H
#define COND_H

int cs_lock(int mutex);
int cs_unlock(int mutex);
int cs_wait(int cond_var, int mutex);
int cs_broadcast(int cond_var);

#endif //COND_H
