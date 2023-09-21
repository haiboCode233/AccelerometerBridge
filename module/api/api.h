#ifndef API_H
#define API_H

#include "module/udp/udp_receiver_qt.h"
#include "module/file_writer/file_writer.h"

class acqlib_api
{
    public:
        acqlib_api();
        ~acqlib_api();
        int acqlib_init();
        float acqlib_get_data(int row, int col);
        int acqlib_write_file();
        int acqlib_deinit();


    public:
        Udp_Receiver_Qt *receiver;
        file_writer *writer;


    private:

};

#endif // API_H
