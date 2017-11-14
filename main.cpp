//
//  main.cpp
//  Project2
//
//  Created by K&R on 11/8/17.
//  Copyright © 2017 K&Rchk. All rights reserved.
//

#include <iostream>

#include "PhysicalMemory.hpp"
#include "BitMap.hpp"


#define FRAME_SIZE 512
#define ST_SIZE 512
#define PT_SIZE 1024
#define PAGE_SIZE 512

int set_page_table(int segment, int pagetable, int *pm,BitMap *bitmap){
    //Also need to update bitmap
    int status = -1;
    
    pm[segment] = pagetable;
    
    //allocate 2 frame for pagetable
    if(pagetable>0){
        bitmap->set_to_one(pagetable/512);
        bitmap->set_to_one(pagetable/512+1);
        status = 1;
    }
    
    return status;
}

int set_page_table_entry(int segment, int page, int f, int *pm, BitMap *bitmap){
    int stauts = -1;
    
    int page_address = pm[segment];
    pm[page_address+page] = f;
    
    if(f>0){
        bitmap->set_to_one(f/512);
        stauts = 1;
    }
    
    return stauts;
}

int get_s(int va){
    int result;
    
    result = va>>19;
    
    return result;
}

int get_p(int va){
    int result;
    
    result = va<<13;
    result = result>>22;
    
    return result;
}

int get_w(int va){
    int result;
    
    result = va << 23;
    result = result >> 23;
    
    return result;
}

int va_read(int va, int* pm){
    int result = -1;
    
    int s = get_s(va);
    int p = get_p(va);
    int w = get_w(va);
    
    if(pm[s] == -1 || pm[pm[s]+p] == -1){
        std::cout << "pf" << std::endl;
        return -1;
    }
    
    if(pm[s] == 0 || pm[pm[s]+p] == 0){
        std::cout << "err" << std::endl;
        return 0;
    }
    
    result = pm[pm[s]+p]+w;
    
    return result;
}

int va_write(int va, int *pm, BitMap *bitmap){
    int result = -1;
    
    int s = get_s(va);
    int p = get_p(va);
    int w = get_w(va);
    
    if(pm[s] == -1 || pm[pm[s]+p] == -1){
        std::cout << "pf" << std::endl;
        return -1;
    }
    
    if(pm[s] == 0){
        int free_frame = bitmap->find_two_empty();
        
        if(free_frame == -1)
            return -1;
        
        bitmap->set_to_one(free_frame);
        bitmap->set_to_one(free_frame+1);
        
        pm[s] = free_frame*512;
        
    }
    
    if(pm[pm[s]+p] == 0){
        int free_frame = bitmap->find_empty();
        
        if(free_frame == -1)
            return -1;
        
        bitmap->set_to_one(free_frame);
        
        pm[pm[s]+p] = free_frame*512;
    }
    
    result = pm[pm[s]+p]+w;
    
    return result;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    PM physical_memory;
    int phym[1024*512];
    BitMap bitmap;
    bitmap.set_to_one(0);
    
    set_page_table(2, 2048, phym,&bitmap);
    set_page_table_entry(2, 0, 512, phym, &bitmap);
    set_page_table_entry(2, 1, -1, phym, &bitmap);
    
    int va = 1049088;
    std::cout << "segment: " << get_s(va) << std::endl;
    std::cout << "page: " << get_p(va) << std::endl;
    std::cout << "w: " << get_w(va) << std::endl;
    
    std::cout << "read va: " << va_read(1048576, phym) << std::endl;
    std::cout << "write va: " << va_write(1048586, phym, &bitmap) << std::endl;
    std::cout << "write va: " << va_write(1574407, phym, &bitmap) << std::endl;
    return 0;
}
