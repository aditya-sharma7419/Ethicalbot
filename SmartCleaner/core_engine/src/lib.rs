use std::ffi::CStr;
use std::os::raw::c_char;
use std::path::Path;
use sysinfo::{ProcessRefreshKind, RefreshKind, System};

const PROTECTED_PATHS: &[&str] = &[
    "c:\\windows", "c:\\program files\\windows defender", "c:\\pagefile.sys",
    "/system", "/usr/bin", "/sbin", "/var/db", "/library/apple",
    "/bin", "/usr/lib", "/etc", "/boot",
    "/.ssh", "/.gnupg", "/.aws"
];

#[no_mangle]
pub extern "C" fn is_path_protected(path_ptr: *const c_char) -> bool {
    if path_ptr.is_null() {
        return true;
    }
    let c_str = unsafe { CStr::from_ptr(path_ptr) };
    let path_str = match c_str.to_str() {
        Ok(s) => s.to_lowercase(),
        Err(_) => return true,
    };

    for protected in PROTECTED_PATHS {
        if path_str.contains(protected) {
            return true;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn is_process_running(proc_name_ptr: *const c_char) -> bool {
    if proc_name_ptr.is_null() {
        return false;
    }
    let c_str = unsafe { CStr::from_ptr(proc_name_ptr) };
    let target_proc = match c_str.to_str() {
        Ok(s) => s.to_lowercase(),
        Err(_) => return false,
    };

    let mut sys = System::new_with_specifics(
        RefreshKind::new().with_processes(ProcessRefreshKind::new()),
    );
    sys.refresh_processes();

    for process in sys.processes().values() {
        if process.name().to_lowercase().contains(&target_proc) {
            return true;
        }
    }
    false
}

#[no_mangle]
pub extern "C" fn trash_item(path_ptr: *const c_char) -> bool {
    if path_ptr.is_null() {
        return false;
    }
    let c_str = unsafe { CStr::from_ptr(path_ptr) };
    let path_str = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return false,
    };

    let path = Path::new(path_str);
    if !path.exists() {
        return false;
    }

    if is_path_protected(path_ptr) {
        return false;
    }

    match trash::delete(path) {
        Ok(_) => true,
        Err(e) => {
            eprintln!("[Rust Core] Trash Error: {:?}", e);
            false
        }
    }
}