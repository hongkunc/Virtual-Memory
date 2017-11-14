//
//  PhysicalMemory.hpp
//  Project2
//
//  Created by K&R on 11/8/17.
//  Copyright © 2017 K&Rchk. All rights reserved.
//

#ifndef PhysicalMemory_hpp
#define PhysicalMemory_hpp

#include <stdio.h>
#include <iostream>

#endif /* PhysicalMemory_hpp */


class PM{

public:
    char pm_array[1024*512];
    
    PM();
};

PM::PM(){
    memset(pm_array,0,sizeof(pm_array));
    std::cout << "Physical Memory initialized!" << std::endl;
}
