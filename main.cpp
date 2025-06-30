#include <memory>
#include <vector>
#include <string_view>
#include <functional>

#include "logging/logger.hpp"
#include "file_reading/file_reader.hpp"
#include "neon_compiler/compiler.hpp"

using namespace std;
using namespace logging;
using namespace neon_compiler;

constexpr const char* TASK_BUILD = "build";
constexpr const char* TASK_ANALYSE = "analyse";

int main(int argc, char** argv)
{
    shared_ptr<Logger> logger = make_shared<Logger>();

    if (argc < 3)
    {
        logger->error("Usage: " + string(argv[0]) + " <build|analyse> <source file(s)>\n");
        return 1;
    }

    Compiler compiler{logger};

    const string_view task{argv[1]};

    function<void(void)> task_runnable;
    if(task == TASK_BUILD)
    {
        task_runnable = bind(&Compiler::build, &compiler);
        logger->info("Building...");
    }
    else if(task == TASK_ANALYSE)
    {
        task_runnable = bind(&Compiler::generate_analysis, &compiler);
        logger->info("Analysing...");
    }
    else
    {
        logger->error("No such task: " + string(task));
        return 1;
    }


    for (int i = 2; i < argc; ++i)
    {
        const char* file_name = argv[i];

        unique_ptr<file_reading::FileReader> file_reader = make_unique<file_reading::FileReader>(logger);

        if (!file_reader->open_file(file_name))
        {
            return 1;
        }

        compiler.read_file(file_reader->move_stream(), string_view{file_name});
    }

    task_runnable();

    return 0;
}
