extern "C" __declspec(dllexport)
int load_dump(const char *f1, const char *f2, char *info);

extern "C" __declspec(dllexport)
int load_dump2(int thread_id, const char *f1, const char *f2, char *info);