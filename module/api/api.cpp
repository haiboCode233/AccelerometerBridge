#include "module/api/api.h"


UDPReceiver *api_receiver;
udp_data *api_data;
file_writer *api_fileWriter;
static bool init_flag = false;


int api_init()
{
    if(!init_flag)
    {
        api_receiver = new UDPReceiver();
        api_data = new udp_data();
        api_fileWriter = new file_writer();
        init_flag = true;
        return 0;
    }
    else
    {
        return -1;
    }

}

int api_deinit()
{
    if(init_flag)
    {
        delete api_receiver;
        delete api_data;
        delete api_fileWriter;
        init_flag = false;
        return 0;
    }
    else
    {
        return -1;
    }
}

int api_getData_once()
{
    if(init_flag)
    {
        api_receiver->startReceive_new(*api_data);
        return 0;
    }
    else
    {
        return -1;
    }

}

int api_show_data()
{
    if(init_flag)
    {
        int row = 0;
        int col = 0;
        for(row=0;row<DATA_ROWS;row++)
        {
            std::cout<<std::endl<<"Channel: "<<row+1 << std::endl;
            for(col=0;col<DATA_COLS;col++)
            {
                std::cout<<std::setiosflags(std::ios::scientific)<<std::setprecision(5)<<api_data->readFromArray()[row][col]<<" \t";
                if(((col+1) % 4 == 0 && col+1 != DATA_COLS) || (row+1 == DATA_ROWS && col+1 == DATA_COLS) )
                {
                    std::cout<<std::endl;
                }
            }
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

int api_getShow()
{
    if(init_flag)
    {
        api_getData_once();
        api_show_data();
        return 0;
    }
    else
    {
        return -1;
    }
}

int api_create_file()
{
    api_fileWriter->outputfile.open(api_fileWriter->outPutFileName, std::ios_base::app);
    return 0;
}

int api_custom_fileName(char *custom_name)
{

    api_fileWriter->outPutFileName = custom_name;
    return 0;
}

int api_auto_fileName()
{

    api_fileWriter->autoFileName();
    return 0;
}

int api_write_file()
{
    if(init_flag)
    {
        api_fileWriter->startwriteFile(*api_data);
        return 0;
    }
    else
    {
        return -1;
    }
}

