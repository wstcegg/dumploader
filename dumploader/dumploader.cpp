#include "stdafx.h"
#include "strconv.h"

#include "MinidumpReader.h"

#include <iostream>
#include "tinyxml.h"


// 创建崩溃行frame节点
TiXmlElement *makeNode_frame(const MdmpStackFrame &frame, const std::vector<MdmpModule> &modules)
{
	TiXmlElement *frameNode = new TiXmlElement("frame");

	strconv_t strconv;
	CString str;

	str.Format(_T("%d"), frame.m_dwAddrPCOffset);
	frameNode->SetAttribute("offset", strconv.t2a(str));

	if (frame.m_nModuleRowID > 0)
		str.Format(_T("%s"), modules[frame.m_nModuleRowID].m_sModuleName);
	else
		str.Format(_T("-1"));
	frameNode->SetAttribute("module", strconv.t2a(str));

	str.Format(_T("%s"), frame.m_sSymbolName);
	frameNode->SetAttribute("symbol", strconv.t2a(str));

	str.Format(_T("%d"), frame.m_dw64OffsInSymbol);
	frameNode->SetAttribute("offset64", strconv.t2a(str));

	str.Format(_T("%s"), frame.m_sSrcFileName);
	frameNode->SetAttribute("srcFile", strconv.t2a(str));

	str.Format(_T("%d"), frame.m_nSrcLineNumber);
	frameNode->SetAttribute("lineNum", strconv.t2a(str));

	return frameNode;
}


// 创建堆栈stack节点
TiXmlElement *makeNode_stack(const MdmpData &dmpData)
{
	const std::vector<MdmpStackFrame> &stack = dmpData.m_CrashStack;
	TiXmlElement *stackNode = new TiXmlElement("stack");

	int num = min(100, int(stack.size()));
	for (int i = 0; i < num; ++i)
	{
		const MdmpStackFrame &frame = stack[i];
		const std::vector<MdmpModule> &modules = dmpData.m_Modules;
		TiXmlElement *frameNode = makeNode_frame(frame, modules);

		if (NULL != frameNode)
			stackNode->LinkEndChild(frameNode);
	}

	// 注意，堆栈节点孩子节点不为空！
	// 否则直接删除当前节点！
	if (stackNode->NoChildren())
	{
		delete stackNode;
		stackNode = NULL;
	}
	return stackNode;
}


// 创建模块名module列表
TiXmlElement *makeNode_module(const std::vector<MdmpModule> &modules)
{
	TiXmlElement *moduleNode = new TiXmlElement("module");

	// 模块名拼接
	CString tmp;
	for (int i = 0; i < modules.size(); ++i)
	{
		tmp += modules[i].m_sModuleName;
		tmp += _T(" ");
	}

	// 插入文本内容
	strconv_t strconv;
	TiXmlText *moduleTextNode = new TiXmlText(strconv.t2a(tmp));
	moduleNode->LinkEndChild(moduleTextNode);

	// 模块名节节点一定会有，此处不作非空检查
	return moduleNode;
}


// MdmpData数据转换为XML格式
bool packXML(const MdmpData &dmpData, TiXmlDocument &d)
{
	// declearation
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0", "UTF-8", "yes");
	d.LinkEndChild(dec);

	// level-1 miniDump
	TiXmlElement *miniDump = new TiXmlElement("miniDump");
	d.LinkEndChild(miniDump);

	// level-2 stack
	TiXmlElement *stackNode = makeNode_stack(dmpData);
	if (NULL == stackNode) return false;
	miniDump->LinkEndChild(stackNode);

	return true;
}


extern "C" __declspec(dllexport)
int load_dump(const char *f1, const char *f2, char *info)
{
	CMiniDumpReader cmr;
	int res = cmr.Open(f1, f2);
	//printf("open succeed");


	// 线程不空
	if (0 == res) {
		//cmr.StackWalk(cmr.m_DumpData.m_uExceptionThreadId);
		try	{
			if (0 == cmr.StackWalk_exception()) {
				//std::cout << "\nthread info"
				//	<< "\t" << cmr.m_DumpData.m_Threads.size()
				//	<< "\t" << cmr.m_DumpData.m_uExceptionThreadId << std::endl;

				// 打包文件
				TiXmlDocument d;
				if (packXML(cmr.m_DumpData, d)) {
					//d.Print();

					//	格式化为字符串
					TiXmlPrinter printer;
					printer.SetIndent("\t");

					d.Accept(&printer);
					std::string s = printer.CStr();

					strcpy(info, s.c_str());
					return 0;
				}
			}
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return 1;
}


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


int _tmain(int argc, _TCHAR* argv[])
{
	char f1[] = "E:\\testInitial\\testMFC\\bin\\4521_892\\275000_285000_4521\\zip\\error_report_275020\\crashdump.dmp";
	char f2[] = "D:\\code\\TOOLS\\released_program\\swc_free_bin_4521\\bin\\release";

	test(f1, f2);

	return 0;
}

