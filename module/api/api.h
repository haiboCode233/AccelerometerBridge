#ifndef API_H
#define API_H
#include <iostream>
#include "module/udp/udp.h"
#include "module/file_writer/file_writer.h"
#include "module/udp/udp_data.h"
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

extern udp_data *api_data;
extern file_writer *api_fileWriter;

int api_init();
int api_deinit();
int api_getData_once();
int api_show_data();
int api_getShow();
int api_create_file();
int api_custom_fileName(char *custom_name);
int api_auto_fileName();
int api_write_file();
#endif // API_H
