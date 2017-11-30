//
//  main.cpp
//  Project2
//
//  Created by K&R on 11/8/17.
//  Copyright © 2017 K&Rchk. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>

#include "BitMap.hpp"
#include "TLB.hpp"


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
    
    if(page_address<=0){
        std::cout <<  "Don't inital with 0" << std::endl;
        return -1;
    }
    
    pm[page_address+page] = f;
    std::cout << "Test: " << pm[page_address+page] << std::endl;
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
    
    result = va & 523776;
    result = result>>9;
    
    return result;
}

int get_w(int va){
    int result;
    
    //result = va << 23;
    result = va & 511;
    
    return result;
}

int get_sp(int va){
    int result;
    
    result = va & (~511);
    result = result >> 9;
    
    return result;
}

int va_read(int va, int* pm){
    int result = -1;
    
    int s = get_s(va);
    int p = get_p(va);
    int w = get_w(va);
    
    if(pm[s] == -1){
        std::cout << "pf" << std::endl;
        return -1;
    }
    
    if(pm[s] == 0){
        std::cout << "err" << std::endl;
        return 0;
    }
    
    if(pm[pm[s]+p] == -1)
        return -1;
    
    if(pm[pm[s]+p] == 0)
        return 0;
    
    result = pm[pm[s]+p]+w;
    
    return result;
}

int va_write(int va, int *pm, BitMap *bitmap){
    int result = -1;
    
    int s = get_s(va);
    int p = get_p(va);
    int w = get_w(va);
    
    if(pm[s] == -1){
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
    
    if(pm[pm[s]+p] == -1)
        return -1;
    
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

int check_tlb(TLB *tlbs, int sp){
    int result = -1;
    
    for(int i=0; i<4; i++){
        if(sp == tlbs[i].s_p)
            return i;
    }
    
    return result;
}

int tlb_hit(TLB *tlbs, int index, int va){
    int result;
    
    int w = get_w(va);
    result = tlbs[index].f+w;
    
    for(int i=0; i<4; i++){
        if(i!=index && tlbs[i].LRU > tlbs[index].LRU && tlbs[i].LRU>0){
            tlbs[i].LRU--;
        }
    }
    
    tlbs[index].LRU = 3;
    
    return result;
}

int tlb_miss(TLB *tlbs, int va, int op, int *pm, BitMap *bitmap){
    int result = 0;
    
    if(op == 0){
        result = va_read(va, pm);
        
        if(result == 0)
            return 0;
        if(result == -1)
            return -1;
    }
    
    if(op == 1){
        result = va_write(va, pm, bitmap);
        
        if(result == -1)
            return -1;
    }
    
    //Update tlb
    int zero_lru_index = -1;
    for(int i=0; i<4; i++){
        if(tlbs[i].LRU == 0)
            zero_lru_index = i;
    }
    if(zero_lru_index != -1){
        tlbs[zero_lru_index].LRU = 3;
        tlbs[zero_lru_index].s_p = get_sp(va);
        tlbs[zero_lru_index].f = result-get_w(va);
        
        for(int i=0; i<4; i++){
            if(i != zero_lru_index && tlbs[i].LRU>0)
                tlbs[i].LRU--;
        }
    }
    
    
    return result;
}

int main(int argc, const char * argv[]) {
    // Initialization
    int phym[1024*512];
    memset(phym, 0, sizeof(phym));
    BitMap bitmap;
    bitmap.set_to_one(0);
    
    
    
    //output file
    std::ofstream output_file;
    std::ofstream output_file2;
    output_file.open("/Users/chenhongkun/Desktop/Virtual\ Memory/Project2/Project2/output61.txt");
    output_file2.open("/Users/chenhongkun/Desktop/Virtual\ Memory/Project2/Project2/output62.txt");
    
    //First Initialization PM
    std:: string line;
    std::string line2;
    std::ifstream input_file;
    
    input_file.open("/Users/chenhongkun/Desktop/Virtual\ Memory/Project2/Project2/input2_1.txt");
    std::getline(input_file,line);
    std::getline(input_file,line2);
    
    std::istringstream iss(line);
    while(!iss.eof()){
        
        std::string word[2];
        iss >> word[0];
        iss >> word[1];
        
        std::cout << word[0] << " " << word[1] << std::endl;
        
        int s,f;
        if(word[0]!="" && word[1]!=""){
            s = std::stoi(word[0]);
            f = std::stoi(word[1]);
            
            set_page_table(s, f, phym, &bitmap);
            //set_page_table(s, f, phym2, &bitmap2);
        }
        
        
    }
    
    std::cout << "511+1001: " << phym[phym[511]+1001] << std::endl;
    
    std::istringstream iss2(line2);
    while(!iss2.eof()){
        
        std::string word[3];
        iss2 >> word[0]; iss2 >> word[1]; iss2 >> word[2];
        
        std::cout << word[0] << " " << word[1] << " " << word[2] << std::endl;
        
        int p,s,f;
        if(word[0]!="" && word[1]!="" && word[2]!=""){
            p = std::stoi(word[0]);
            s = std::stoi(word[1]);
            f = std::stoi(word[2]);
            
            set_page_table_entry(s, p, f, phym, &bitmap);
            //set_page_table_entry(s, p, f, phym2, &bitmap2);
        }
        
        
    }
    
    
    //VM translation without TLB
    std::string t_line;
    std::ifstream input_file2;
    input_file2.open("/Users/chenhongkun/Desktop/Virtual\ Memory/Project2/Project2/input2_2.txt");
    
    std::getline(input_file2,t_line);
    std::istringstream t_iss(t_line);
    while(!t_iss.eof()){
        
        std::string word[2];
        t_iss >> word[0]; t_iss >> word[1];
        
        std::cout << word[0] << " " << word[1] << std::endl;
        
        
        if(word[0]!="" && word[1]!=""){
            int o = std::stoi(word[0]);
            int va = std::stoi(word[1]);
            
            if(o == 0){
                int result = va_read(va, phym);
                if(result==0)
                    output_file << "err ";
                if(result==-1)
                    output_file << "pf ";
                if(result>0){
                    output_file << result;
                    output_file << " ";
                }
                
            }
            else if(o == 1){
                int result = va_write(va, phym, &bitmap);
                
                //std::cout << "511+1001: " << phym[phym[511]+1001] << std::endl;
                
                if(result == -1)
                    output_file << "pf ";
                else{
                    output_file << result;
                    output_file <<  " ";
                }
            }
            else
                std::cout << "error" << std::endl;
        }
    }
    
    get_w(1573375);
    get_p(409088);
    get_s(267911168);
    get_sp(267911680);
    
    //Second initialization
    memset(phym, 0, sizeof(phym));
    BitMap bitmap2;
    bitmap2.set_to_one(0);
    TLB tlbs[4];
    
    std::istringstream iss3(line);
    while(!iss3.eof()){
        
        std::string word[2];
        iss3 >> word[0];
        iss3 >> word[1];
        
        std::cout << word[0] << " " << word[1] << std::endl;
        
        int s,f;
        if(word[0]!="" && word[1]!=""){
            s = std::stoi(word[0]);
            f = std::stoi(word[1]);
            
            set_page_table(s, f, phym, &bitmap2);
            //set_page_table(s, f, phym2, &bitmap2);
        }
        
        
    }
    
    std::istringstream iss4(line2);
    while(!iss4.eof()){
        
        std::string word[3];
        iss4 >> word[0]; iss4 >> word[1]; iss4 >> word[2];
        
        std::cout << word[0] << " " << word[1] << " " << word[2] << std::endl;
        
        int p,s,f;
        if(word[0]!="" && word[1]!="" && word[2]!=""){
            p = std::stoi(word[0]);
            s = std::stoi(word[1]);
            f = std::stoi(word[2]);
            
            set_page_table_entry(s, p, f, phym, &bitmap2);
            //set_page_table_entry(s, p, f, phym2, &bitmap2);
        }
        
        
    }

    
    
    //Translation with TLB
    std::istringstream t_iss2(t_line);
    while(!t_iss2.eof()){
        
        std::string word[2];
        t_iss2 >> word[0]; t_iss2 >> word[1];
        
        std::cout << word[0] << " " << word[1] << std::endl;
        
        if(word[0]!="" && word[1]!=""){
            int o = std::stoi(word[0]);
            int va = std::stoi(word[1]);
            
            int check_result = check_tlb(tlbs, get_sp(va));
            
            if(check_result != -1){
                int result = tlb_hit(tlbs, check_result, va);
                output_file2 << "h ";
                output_file2 << result;
                output_file2 << " ";
            }else{
                int result = tlb_miss(tlbs, va, o, phym, &bitmap2);
                
                if(result == 0)
                    output_file2 << "m err ";
                else if(result == -1)
                    output_file2 << "m pf ";
                else{
                    output_file2 << "m ";
                    output_file2 << result;
                    output_file2 << " ";
                }
            }
            
        }
    }
    
    input_file.close();
    input_file2.close();
    output_file.close();
    output_file2.close();
    
    return 0;
}
