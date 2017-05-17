/*
 * clouddata.cpp
 *
 *  Created on: May 17, 2017
 *      Author: elite
 */

#include <git/clouddata.h>
#include <stdint.h>

cloud_data::cloud_data()
{
    // TODO Auto-generated constructor stub
    this->rele1state = 0;

}

cloud_data::~cloud_data()
{
    // TODO Auto-generated destructor stub
}

uint8_t cloud_data::get_ToggleRealyData()
{
    return this->rele1state;
}
