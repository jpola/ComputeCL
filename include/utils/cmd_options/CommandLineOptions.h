#ifndef _CMD_LINE_OPT_H
#define _CMD_LINE_OPT_H

#include <boost/program_options.hpp>

namespace computecl
{

    bool ParseCommandLine(int argc, char* argv[],
                          std::string& vendor,
                          std::string& matrix_file)
    {
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help", "produce help message")
                ("vendor", po::value<std::string>(), "Choose device vendor NVIDIA or AMD")
                ("mtx", po::value<std::string>(), "Input matrix file in MTX format");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        std::cout << "Hello Boost Compute!" << std::endl;

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }


        //std::string vendor;
        if (vm.count("vendor"))
        {
            std::string v = vm["vendor"].as<std::string>();

            if (v == "AMD")
            {
                vendor = "Advanced Micro Devices, Inc.";
                std::cout << "Choosing device from "
                          << vendor << std::endl;
            }

            else if (v == "NVIDIA")
            {
                vendor = "NVIDIA Corporation";

                std::cout << "Choosing device from "
                          << vendor << std::endl;
            }

            else
            {
                std::cout << "Wrong vendor specified,";
                std::cout << " please select from NVIDIA or AMD vendors";
                std::cout << " as --vendor parameter" <<std::endl;
                return false;
            }

        }

        if(vm.count("mtx"))
        {
            matrix_file = vm["mtx"].as<std::string>();
        }
        else
        {
            std::cout << "No input matrix file specified" << std::endl;
            return false;
        }

        return true;
    }

} //computecl

#endif //cmdoptions
