#ifndef RUST_CORE_H
#define RUST_CORE_H

extern "C"
{
    bool is_path_protected(const char *path);
    bool is_process_running(const char *proc_name);
    bool trash_item(const char *path);
}

#endif // RUST_CORE_H