#include <cclog.hpp>
#include <string>
#include <iostream>
#include <thread>

// init
// uninit
// resetloglevel
// addloglevel
// log
// set_default_message_format
// last_log_retval
// set_verbostiy_level
// export/import config
// Server stuff

const std::string conf_path = "/tmp/testconfig.json";

int callback_test(const char *msg, void *priv){
    return 10;;
}

static cclog::cclog_callback_mapping_t cbs[] = {
    {"callback_test", callback_test},
    {nullptr, nullptr}
};

void make_config() {
        // add one custom log level
        cclog::reset_log_levels();
        cclog::add_log_level(true, true, cclog::CCLOG_TTY_CLR_BLU, 10, &cbs[0], "Callback ${UPTIME} ${MSG}");
        // since we cleared the log level list this level will have index of 0
        cclog::add_log_level(true, true, cclog::CCLOG_TTY_CLR_MAG, 10);
        // set default format
        cclog::set_default_message_format("${DATE} ${TIME} ${FILE} ${LINE} - ${MSG}");
        cclog::set_verbosity_level(15);
        cclog::export_config(conf_path);
}

int main() {
    // initialise the logger
    try {
        cclog::init(cclog::LOGGING_SINGLE_FILE, "/tmp/testlog", "test_proc_name");

        //make_config();
        cclog::load_config(conf_path, cbs);

        cclog::server_start(8000);

        cclog::log(1, "Server pid is " + std::to_string(cclog::server_pid()));
        cclog::log(1, "Performing operation...");

        int result = cclog::log(0, "Operating...");
        if (result == 10) {
            cclog::log(0, "Operation successfull: " + std::to_string(result));
        } else {
            cclog::log(0, "Operation failed, expected 10, got: " + std::to_string(result));
        }

        std::cout << cclog::last_log_retval() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        cclog::server_stop();

        // uninit the logger
        cclog::uninit();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

