#include <iostream>
#include <chrono>

#include "clipp.h"
#include "templatedb/db.hpp"
#include "templatedb/operation.hpp"

typedef struct _configuration
{
    std::string dbname;
    std::string fname;
    std::string wlname;
} config;


config cmdline_settings(int argc, char * argv[])
{
    using namespace clipp;
    config cfg;
    auto cli = (
        (value("database", cfg.dbname)),
        (option("-f", "--file")
            % "(ingest data file)"
            & value("file", cfg.fname)),
        (required("-w", "--work")
            % "(input workload file)"
            & value("workload", cfg.wlname))
    );

    if (!parse(argc, argv, cli)) {
        std::cout << make_man_page(cli, "simple_benchmark");
        exit(EXIT_FAILURE);
    }

    return cfg;
}


int main(int argc, char * argv[])
{
    config cfg = cmdline_settings(argc, argv);

    templatedb::DB db;
    auto start = std::chrono::high_resolution_clock::now();
    if (db.open(cfg.dbname) != templatedb::OPEN)
    {
        fprintf(stderr, "Unable to load DB %s\n", cfg.dbname.c_str());
        exit(EXIT_FAILURE);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("Insert mem_table Time %d us\n", duration);
    
    if (!cfg.fname.empty())
    {
        start = std::chrono::high_resolution_clock::now();
        if (!db.load_data_file(cfg.fname))
        {
            fprintf(stderr, "Unable to load data file %s into DB\n", cfg.fname.c_str());
            if (db.status == templatedb::OPEN)
                db.close();
            exit(EXIT_FAILURE);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        printf("Load Data Time %d us\n", duration);
    }

    std::vector<templatedb::Operation> ops = templatedb::Operation::ops_from_file(cfg.wlname);
    int cnt = 0;

    start = std::chrono::high_resolution_clock::now();
    for (auto op : ops)
    {
        db.execute_op(op);
        if (++cnt % 50 == 0)
            std::cout << cnt << std::endl;
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    printf("Workload Time %d us\n", duration);

    if (db.status == templatedb::OPEN)
        db.close();

    return 0;
}