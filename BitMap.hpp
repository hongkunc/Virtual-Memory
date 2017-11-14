//
//  BitMap.hpp
//  Project2
//
//  Created by K&R on 11/8/17.
//  Copyright © 2017 K&Rchk. All rights reserved.
//

#ifndef BitMap_hpp
#define BitMap_hpp

#include <stdio.h>
#include <iostream>

#endif /* BitMap_hpp */

class BitMap{
    
public:
    int bitmap[32];
    int mask[32];
    int mask2[32];
    
    BitMap();
    
    int find_empty();
    int find_two_empty();
    int set_to_one(int position);
    int set_to_zero(int position);
};

BitMap::BitMap(){
    memset(bitmap, 0, sizeof(bitmap));
    
    mask[31] = 1;
    for(int i=0; i<31; i++){
        mask[30-i] = mask[30-i+1]<<1;
    }
    
    for(int i=0; i<32; i++){
        mask2[i] = ~mask[i];
    }
}

int BitMap::find_empty(){
    int result = -1;
    
    for(int i=0; i<32; i++){
        for(int j=0; j<32; j++){
            int test = bitmap[i]&mask[j];
            if(test == 0)
                return i*32+j;
        }
    }
    
    return result;
}

int BitMap::find_two_empty(){
    //return the starting empty frame
    int result = -1;
    int test1=0, test2;
    
    for(int i=0; i<32; i++){
        for(int j=0; j<32; j++){
            test1 = bitmap[i]&mask[j];
            if(test1 == 0){
                result = i*32+j;
                if(j==31 && i<31){
                    test2 = bitmap[i+1]&mask[0];
                }else{
                    test2 = bitmap[i]&mask[j+1];
                }
                
                if(test2 == 0)
                    return result;
            }
            
            
        }
    }
    
    return result;
}

int BitMap::set_to_one(int position){
    int status = 1;
    
    int index = position/32;
    int offset = position%32;
    
    bitmap[index] = bitmap[index]|mask[offset];
    
    
    return status;
}
