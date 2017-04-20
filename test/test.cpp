#include <iostream>

#include "dumploader.h"

using namespace std;

int test(char *f1, char *f2)
{
	std::cout << f1 << "\n" << f2 << std::endl;
	char *info = new char[1280000];

	int res = load_dump(f1, f2, info);
	std::cout << "result: " << res << std::endl;
	if (0 == res) std::cout << info << std::endl;
	delete info;

	return 0;
}

int main(int argc, char *argv[])
{
	char file1[] = "E:\\testInitial\\testMFC\\bin\\4521_892\\275000_285000_4521\\zip\\error_report_275020\\crashdump.dmp";
	char file2[] = "D:\\code\\TOOLS\\released_program\\swc_free_bin_4521\\bin\\release";

	char *f1 = file1;
	char *f2 = file2;

	if (argc >= 3){
		f1 = argv[1];
		f2 = argv[2];
	}

	test(f1, f2);
	return 0;
}