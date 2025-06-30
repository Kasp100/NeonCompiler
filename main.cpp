#include <memory>
#include <vector>
#include <string_view>
#include <functional>

#include "logging/logger.hpp"
#include "file_reading/file_reader.hpp"
#include "neon_compiler/compiler.hpp"

constexpr const char* TASK_BUILD = "build";
constexpr const char* TASK_ANALYSE = "analyse";

int main(int argc, char** argv)
{
    std::shared_ptr<logging::Logger> logger = std::make_shared<logging::Logger>();

    if (argc < 3)
    {
        logger->error("Usage: " + std::string(argv[0]) + " <build|analyse> <source file(s)>\n");
        return 1;
    }

    neon_compiler::Compiler compiler{logger};

    const std::string_view task{argv[1]};

    std::function<void(void)> task_runnable;
    if(task == TASK_BUILD)
    {
        task_runnable = std::bind(&neon_compiler::Compiler::build, &compiler);
        logger->info("Building...");
    }
    else if(task == TASK_ANALYSE)
    {
        task_runnable = std::bind(&neon_compiler::Compiler::generate_analysis, &compiler);
        logger->info("Analysing...");
    }
    else
    {
        logger->error("No such task: " + std::string(task));
        return 1;
    }

    for (int i = 2; i < argc; ++i)
    {
        const char* file_name = argv[i];

        std::unique_ptr<file_reading::FileReader> file_reader = std::make_unique<file_reading::FileReader>(logger);

        if (!file_reader->open_file(file_name))
        {
            return 1;
        }

        compiler.read_file(file_reader->move_stream(), std::string_view{file_name});
    }

    task_runnable();

    return 0;
}
