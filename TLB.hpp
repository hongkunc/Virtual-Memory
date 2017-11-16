//
//  TLB.hpp
//  Project2
//
//  Created by K&R on 11/14/17.
//  Copyright © 2017 K&Rchk. All rights reserved.
//

#ifndef TLB_hpp
#define TLB_hpp

#include <stdio.h>
#include <iostream>

#endif /* TLB_hpp */

class TLB{
    
public:
    int LRU;
    int s_p;
    int f;
    
    TLB();
    int set_up(int lru, int sp, int w);
    
    
};

TLB::TLB(){
    LRU = 0;
    s_p=-1;
    f=0;
}

int TLB::set_up(int lru, int sp, int w){
    int status = -1;
    
    LRU = lru;
    s_p = sp;
    f = w;
    
    return status;
}
