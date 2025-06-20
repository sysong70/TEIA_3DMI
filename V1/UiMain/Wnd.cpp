#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Wnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	using Extensions = std::vector<CString>;

	Extensions Extensions3d;
	Extensions Extensions2d;

	Extensions& Get3dExtensions()
	{
		// generate extensions
		if (Extensions3d.size() == 0) {
			Json::Array& items = TheAppResources
				.GetDialog("FileOptions")
				.GetArray("tree")
				.GetObject(0)
				.GetArray("items");

			for (auto item : items.GetBuffer()) {
				Json::Object& target = item->AsObject();
				if (target.GetBoolean("visible", true) == false) {
					continue;
				}

				Json::Array& extensions = target.GetArray("ext");
				for (auto ext : extensions.GetBuffer()) {
					Extensions3d.push_back(ext->AsString());
				}
			}

			// #sysong 
			// Point Cloud Data 
			Extensions3d.push_back(L"PTS");
			Extensions3d.push_back(L"PTX");
			Extensions3d.push_back(L"XYZ");
			// KEN - move to 2d
			//Extensions3d.erase(std::remove(Extensions3d.begin(), Extensions3d.end(), L"DWG"));
			//Extensions3d.erase(std::remove(Extensions3d.begin(), Extensions3d.end(), L"DXF"));
		}

		return Extensions3d;
	}

	Extensions& Get2dExtensions()
	{
		// TODO
		if (Extensions2d.size() == 0) {
			Extensions2d.push_back(L"DWG");
			Extensions2d.push_back(L"DXF");
		}

		return Extensions2d;
	}
}

//**************************************************************************************************

bool Wnd::IsAllowedFile(const wchar_t* pFilePath)
{
	return IsAllowed3d(pFilePath) || IsAllowed2d(pFilePath);
}

// REF - https://docs.techsoft3d.com/exchange/latest/start/supported-formats.html

bool Wnd::IsAllowed3d(const wchar_t* pFilePath)
{
	CString extension = Path::GetExtension(pFilePath);
	extension.MakeUpper();

	// CHECK - how to 2d/3d?
	if (extension == L"DWG" || extension == L"DXF") {
		return false;
	}

	// HSF is 3D format: #HSF
	if (L"HSF" == extension) {
		return true;
	}

	// WARNING - Creo/ProE (case *.1)
	if (WStr::IsDigit(extension)) {
		return true;
	}

	for (auto& item : Get3dExtensions()) {
		if (item == extension) {
			return true;
		}
	}

	return false;
}



bool Wnd::IsAllowed2d(const wchar_t* pFilePath)
{
	CString extension = Path::GetExtension(pFilePath);
	extension.MakeUpper();

	for (auto& item : Get2dExtensions()) {
		if (item == extension) {
			return true;
		}
	}

	return true;
}
