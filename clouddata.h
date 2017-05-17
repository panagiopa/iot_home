/*
 * clouddata.h
 *
 *  Created on: May 17, 2017
 *      Author: elite
 */

#ifndef GIT_CLOUDDATA_H_
#define GIT_CLOUDDATA_H_

#include <stdint.h>

class cloud_data
{
    public:
        cloud_data();
        virtual ~cloud_data();
        uint8_t get_ToggleRealyData();

    private:
        uint8_t rele1state;
};

#endif /* GIT_CLOUDDATA_H_ */
