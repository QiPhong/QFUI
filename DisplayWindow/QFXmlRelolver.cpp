#include "QFXmlRelolver.h"
#include <windows.h>
#include "QFTypes.h"





namespace QFUSE
{
	PICPATHCONVERTOR g_picPathCtor = nullptr;
	std::wstring stringToWString(const std::string& str) {
		// ����ת������Ļ�������С
		int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
		std::wstring wstr(size_needed, 0);

		// ִ��ת��
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);

		return wstr;
	}
	std::string wstring_to_string(const std::wstring& wstr) {
		if (wstr.empty()) {
			return std::string();
		}

		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
		return str;
	}
	//std::map<std::string, int> g_connectFun;
	

	//ȡ������ַ
	void* GetEventFun(const char* name)
	{
		/*std::string fn(name);
		auto it = g_connectFun.find(fn);
		if (it == g_connectFun.end())return nullptr;


		return (void*)it->second;*/
		//return nullptr;
		return (void*)FindEventFun(name);
	}
	QRect XML2Rect(const std::string& data) {
		QRect rect;
		std::regex pattern(R"(\((\d+),(\d+),(\d+),(\d+)\))");
		std::smatch matches;
		int ci[4];
		if (std::regex_match(data, matches, pattern)) {
			if (matches.size() == 5) { // matches[0] is the whole match, matches[1] to matches[4] are the groups
				for (size_t i = 1; i < matches.size(); ++i) {
					ci[i - 1] = std::stoi(matches[i].str());
				}
			}
		}
		rect.X = ci[0];
		rect.Y = ci[1];
		rect.Width = ci[2];
		rect.Height = ci[3];
		return rect;
	}
	QPointF XML2Point(const std::string& data) {
		QPointF point;
		// ����������ʽģʽ��ƥ��������С����ʽ�������
		std::regex pattern(R"(\(\s*([-+]?\d*\.?\d+)\s*,\s*([-+]?\d*\.?\d+)\s*\))");

		// ����һ��smatch�������ڴ洢ƥ����
		std::smatch match;

		// ���ҵ�һ��ƥ����
		if (std::regex_search(data, match, pattern)) {

			// ��ȡ�����x��y��ֵ
			point.X = std::stod(match[1].str());
			point.Y = std::stod(match[2].str());

		}
		return point;
	}

	void* XMLCreateButton(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ��������ť����
		QButton* qct = (QButton*)QFUSE::CreateQWND("QFButton", rect);
		if (!qct)return qct;


		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ���ť��������
				qct->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//��Ӹ���ť�����ı�
				qct->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("onClick" == na) {
				qct->SetClickEvent(QFBUTTONCLICK(GetEventFun(attr->Value())));
			}
			else if ("fontName" == na) {
				qct->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				qct->SetFontSize(std::stoi(attr->Value()));
			}

			attr = attr->Next();
		}

		return qct;//���ذ�ťָ��
	}
	void* XMLCreateButton2(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ��������ť����
		QButton2* qct = (QButton2*)QFUSE::CreateQWND("QFPButton", rect);
		if (!qct)return qct;


		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ���ť��������
				qct->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//��Ӹ���ť�����ı�
				qct->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("onClick" == na) {
				qct->SetClickEvent(QFBUTTONCLICK(GetEventFun(attr->Value())));
			}
			else if ("fontName" == na) {
				qct->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				qct->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				qct->SetFontColor(color);

			}
			else if ("bkColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				qct->SetBKColor(color);

			}
			attr = attr->Next();
		}

		return qct;//���ذ�ťָ��
	}
	void* XMLCreateEdit(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������༭�����
		QEdit* ctrl = (QEdit*)QFUSE::CreateQWND("QFEdit", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//��Ӹ��༭�������ı�
				ctrl->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("TextChange" == na) {
				ctrl->SetChangeEvent(QEDIT_CHANGE_EVENT(GetEventFun(attr->Value())),0);
		
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateReportForm(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������������
		QReportForm* ctrl = (QReportForm*)QFUSE::CreateQWND("QFReportForm", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("IsDrag" == na) {
				std::string str(attr->Value());
				std::transform(str.begin(), str.end(), str.begin(),
					[](unsigned char c) { return std::tolower(c);
					});
				if ("true" == str) {
					ctrl->IsIsDrag(true);
				}
				else {
					ctrl->IsIsDrag(false);
				}
			}
			else if ("TitleHeight" == na) {
				ctrl->SetTitleHeight(std::stoi(attr->Value()));
			}

			attr = attr->Next();
		}
		//��������
		tinyxml2::XMLElement* FTitle = ele->FirstChildElement("FTitle");
		if (FTitle) {
			for (tinyxml2::XMLElement* child = FTitle->FirstChildElement("FItem"); child != nullptr; child = child->NextSiblingElement("FItem")) {

				const tinyxml2::XMLAttribute* name = child->FindAttribute("text");
				const tinyxml2::XMLAttribute* width = child->FindAttribute("width");
				if (name && width) {
					//�ÿ�Ⱥ��ı�����һ������
					ctrl->InsertTitle(stringToWString(name->Value()), std::stoi(width->Value()));
				}
			}

		}



		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreatePictureBox(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ������ͼƬ�����
		QPictureBox* ctrl = (QPictureBox*)QFUSE::CreateQWND("QFPictureBox", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("path" == na) {
				//��Ӹ�ͼƬ�����ͼƬ����·������
				std::string spath(attr->Value());
				std::wstring path = stringToWString(spath);

				if (g_picPathCtor) {
					path = g_picPathCtor(spath.c_str());
				}
				ctrl->LoadPicture(path);
			}
			else if ("Transparency" == na) {
				unsigned char tp = 255 * std::stod(attr->Value());
				//ctrl->SetTransparency(tp);
				ctrl->SetTransparency(tp);
			}

			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateProgressBarCar(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ���������ͽ���������
		QProgressBarCar* ctrl = (QProgressBarCar*)QFUSE::CreateQWND("QFProgressBarCar", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("posit" == na) {
				//��Ӹ����ͽ��������ý���
				ctrl->SetPercentage(std::stod(attr->Value()));
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateLineChart(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ����������ͼ�����
		QLineChart* ctrl = (QLineChart*)QFUSE::CreateQWND("QFLineChart", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("legfontName" == na) {
				ctrl->SetLegFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("legfontSize" == na) {
				ctrl->SetLegFontSize(std::stoi(attr->Value()));
			}
			else if ("XAxisStrHeight" == na) {
				ctrl->SetXAxisStrHeight(std::stod(attr->Value()));
			}
			else if ("YAxisStrWidth" == na) {
				ctrl->SetYAxisStrWidth(std::stod(attr->Value()));
			}
			else if ("XAxisScaleWidth" == na) {
				ctrl->SetXAxisScaleWidth(std::stod(attr->Value()));
			}
			else if ("YAxisScaleWidth" == na) {
				ctrl->SetYAxisScaleWidth(std::stod(attr->Value()));
			}
			else if ("LineWidth" == na) {
				ctrl->SetLineWidth(std::stod(attr->Value()));
			}
			else if ("XAxisRange" == na) {
				QPointF point = XML2Point(attr->Value());
				ctrl->SetXAxisRange(point.X, point.Y);
			}
			else if ("YAxisRange" == na) {
				QPointF point = XML2Point(attr->Value());
				ctrl->SetYAxisRange(point.X, point.Y);
			}
			else if ("LegWidth" == na) {
				ctrl->SetLegWidth(std::stod(attr->Value()));
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateTable(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ��������ҳ��ǩ����
		QTabs* ctrl = (QTabs*)QFUSE::CreateQWND("QFTabs", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			attr = attr->Next();
		}
		//��ǩ����
		tinyxml2::XMLElement* Tables = ele->FirstChildElement("Tables");
		if (Tables) {
			int index = 0;
			for (tinyxml2::XMLElement* child = Tables->FirstChildElement("Table"); child != nullptr; child = child->NextSiblingElement("Table"), ++index) {

				const tinyxml2::XMLAttribute* text = child->FindAttribute("text");
				if (text) {
					//����һ����ǩ
					//�Ƿ���ͼ��
					const tinyxml2::XMLAttribute* ico = child->FindAttribute("ico");
					if (ico) {
						std::string spath(ico->Value());
						std::wstring path = stringToWString(spath);

						if (g_picPathCtor) {
							path = g_picPathCtor(spath.c_str());
						}
						//����һ����ǩ(��ͼ���)
						ctrl->AddTab(stringToWString(text->Value()).c_str(), path.c_str());

					}
					else {
						ctrl->AddTab(stringToWString(text->Value()).c_str());
					}
					//�鿴��û�и��ӿؼ�
					for (tinyxml2::XMLElement* childCtrl = child->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
						//���ش���һ���ؼ�
						DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl);
						//����ؼ�ָ�벻Ϊ�գ�����뵱ǰ��ǩ
						if (p) {
							ctrl->AttachCtrl(p, index);
						}

					}

				}





			}

		}


		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateCATLBarChart(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QCATLBarChart* ctrl = (QCATLBarChart*)QFUSE::CreateQWND("QFCATLBarChart", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("ScaleColor" == na) {
				ctrl->SetScaleColor(std::stoi(attr->Value()));
			}
			else if ("ScaleMax" == na) {
				ctrl->SetScaleMax(std::stof(attr->Value()));
			}
			else if ("ScaleMin" == na) {
				ctrl->SetScaleMin(std::stof(attr->Value()));
			}
			else if ("ScaleWidth" == na) {
				ctrl->SetScaleWidth(std::stof(attr->Value()));
			}
			else if ("ScaleHeight" == na) {
				ctrl->SetScaleHeight(std::stof(attr->Value()));
			}
			else if ("BarChartWidth" == na) {
				ctrl->SetBarChartWidth(std::stof(attr->Value()));
			}
			else if ("GridWidth" == na) {
				ctrl->SetGridWidth(std::stof(attr->Value()));
			}
			else if ("LineWidth" == na) {
				ctrl->SetLineWidth(std::stof(attr->Value()));
			}
			else if ("YAxisStrWidth" == na) {
				ctrl->SetYAxisStrWidth(std::stof(attr->Value()));
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateCATLTempeChart(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QCATLTempeChart* ctrl = (QCATLTempeChart*)QFUSE::CreateQWND("QFCATLTempeChart", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("UnitWidth" == na) {
				ctrl->SetUnitWidth(std::stod(attr->Value()));
			}
			else if ("UnitFontSize" == na) {
				ctrl->SetUnitFontSize(std::stod(attr->Value()));
			}
			else if ("UnitIntervalWidth" == na) {
				ctrl->SetUnitIntervalWidth(std::stod(attr->Value()));
			}
			else if ("AlignWidth" == na) {
				ctrl->SetAlignWidth(std::stod(attr->Value()));
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateDrawer(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QDrawer* ctrl = (QDrawer*)QFUSE::CreateQWND("QFDrawer", rect);
		if (!ctrl)return ctrl;

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("onSelected" == na) {
				ctrl->SetSelectEvent(QDRAWERSELECT(GetEventFun(attr->Value())));
			}
			attr = attr->Next();
		}
		//��Ŀ����
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			int index = 0;
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("Category"); child != nullptr; child = child->NextSiblingElement("Category"), ++index) {
				ctrl->InsertDrawer(-1);
				int id = ctrl->GetDrawerCount()-1;
				const tinyxml2::XMLAttribute* name = child->FindAttribute("name");
				if (name) {
					std::string v(name->Value());
					ctrl->SetDrawerName(id, stringToWString(v).c_str());
				}
				for (tinyxml2::XMLElement* Subkey = child->FirstChildElement("Subkey"); Subkey != nullptr; Subkey = Subkey->NextSiblingElement("Subkey")) {
				
					const tinyxml2::XMLAttribute* nm = Subkey->FindAttribute("name");
					if (nm) {
						std::string text = nm->Value();

						ctrl->InsertSubObject(id, -1);
						int sid = ctrl->GetSubObjectCount(id) - 1;
						ctrl->SetSubObjectName(id, sid, stringToWString(text).c_str());

					}
				
				
				
				}

			}

		}



		return ctrl;//���ذ�ťָ��
	}

	void* XMLCreateHLayout(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ������ˮƽ����������
		QHLayout* ctrl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", rect);
		if (!ctrl)return ctrl;



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}

			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {
				unsigned int weight = 0;
				auto cattri = childCtrl->FindAttribute("weight");
				if (cattri) {
					weight = std::strtoul(cattri->Value(), nullptr, 10);
				}
				ctrl->InsertCtrl(p, weight);

			}

		}


		return ctrl;//���ذ�ťָ��
	}

	void* XMLCreateVLayout(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ��������ֱ����������
		QVLayout* ctrl = (QVLayout*)QFUSE::CreateQWND("QFVLayout", rect);
		if (!ctrl)return ctrl;
		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}

			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {
				unsigned int weight = 0;
				auto cattri = childCtrl->FindAttribute("weight");
				if (cattri) {
					weight = std::strtoul(cattri->Value(), nullptr, 10);
				}
				ctrl->InsertCtrl(p, weight);
			}


		}


		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateTLayout(tinyxml2::XMLElement* ele)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ��������ֱ����������
		QTLayout* ctrl = (QTLayout*)QFUSE::CreateQWND("QFTLayout", rect);
		if (!ctrl)return ctrl;
		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}

			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {


				ctrl->SetCtrl(p->qwm);
			}


		}


		return ctrl;//���ذ�ťָ��
	}

	void* XMLCreateStaticText(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QStaticText* ctrl = (QStaticText*)QFUSE::CreateQWND("QFStaticText", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//���þ�̬�ı����ı�
				ctrl->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			else if ("backgroundColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetBKColor(color);
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateSlider(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QSlider* ctrl = (QSlider*)QFUSE::CreateQWND("QFSlider", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Direction" == na) {
				//���þ�̬�ı����ı�
				ctrl->SetDirection(std::strtol(attr->Value(), nullptr, 10));
			}
			else if ("IsSegmentation" == na) {
				if ("true" == value) {
					ctrl->IsSegmentation(true);
				}
				else {
					ctrl->IsSegmentation(false);
				}

			}
			else if ("RailColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);

				ctrl->SetRailColor(color);
			}
			else if ("RailRatio" == na) {
				double rat = std::strtod(attr->Value(), nullptr);
				ctrl->SetRailRatio(rat);
			}
			else if ("backgroundColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetColor(color);
			}
			else if ("maxValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetMaxValue(v);
			}
			else if ("minValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetMinValue(v);
			}
			else if ("currentValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetCurrentValue(v);

			}
			else if ("ValueChangeEvent" == na) {
				//�����ֵ�ı亯���ص�����
				ctrl->SetValueChangeEvent((QSLIDER_VALUE_CHANGE)GetEventFun(attr->Value()));
			}
			attr = attr->Next();
		}
		const tinyxml2::XMLAttribute* currentValue = ele->FindAttribute("currentValue");
		if (currentValue) {
			double v = std::strtod(currentValue->Value(), nullptr);
			ctrl->SetCurrentValue(v);
		
		}

		//�ֶ�ֵ
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("SegmValue"); child != nullptr; child = child->NextSiblingElement("SegmValue")) {

				const tinyxml2::XMLAttribute* value = child->FindAttribute("value");

				if (value) {
					//����һ���ֶ�
					ctrl->AddSegmVuale(std::strtod(value->Value(), nullptr));
				}
			}

		}


		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateSwitch(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QSwitch* ctrl = (QSwitch*)QFUSE::CreateQWND("QFSwitch", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("IsMutex" == na) {
				if ("true") {
					ctrl->IsMutex(true);
				}
				else {
					ctrl->IsMutex(false);
				}

			}
			else if ("State" == na) {
				if ("true") {
					ctrl->SetState(true);
				}
				else {
					ctrl->SetState(false);
				}
			}
			else if ("StateChangeEvent" == na) {
				//���ת̬�䶯
				ctrl->SetStateChangeEvent((QSWITCH_STATE_CHANGE)GetEventFun(attr->Value()));
			}

			attr = attr->Next();
		}
		
		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateProgress(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QProgress* ctrl = (QProgress*)QFUSE::CreateQWND("QFProgress", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();
		unsigned int c1 = 0, c2 = 0;
		int rcCount = 0;
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("CurrentValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetCurrentValue(v);
			}
			else if ("DecimalPlaces" == na) {

				unsigned char v = std::strtoul(attr->Value(), nullptr, 10);
				ctrl->SetDecimalPlaces(v);
			}
			else if ("Direction" == na) {
				unsigned char v = std::strtoul(attr->Value(), nullptr, 10);
				ctrl->SetDirection(v);
			}
			else if ("FontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			else if ("LineTypeColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetLineTypeProgressColor(color);
			}
			else if ("RingTypeColor1" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				c1 = std::strtoul(attr->Value(), nullptr, para);
				++rcCount;
				if (rcCount > 1)
					ctrl->SetRingProgressColor(c1, c2);
			}
			else if ("RingTypeColor2" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				c2 = std::strtoul(attr->Value(), nullptr, para);
				++rcCount;
				if (rcCount > 1)
					ctrl->SetRingProgressColor(c1, c2);
			}
			else if ("FontName" == na) {

				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("FontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
			else if ("RingProgressThickness" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetRingProgressThickness(v);
			}
			else if ("Type" == na) {
				unsigned int v = std::strtoul(attr->Value(), nullptr, 10);
				ctrl->SetStyle(v);
			}
			attr = attr->Next();
		}
	
		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateColorBox(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QColorBox* ctrl = (QColorBox*)QFUSE::CreateQWND("QFColorBox", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {

				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("fontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
			else if ("ColorChangeEvent" == na) {
				ctrl->SetColorChangeEvent((QCOLORBOX_CHANGE)GetEventFun(attr->Value()));
				
			}
			attr = attr->Next();
		}
		return ctrl;//���ذ�ťָ��
	}
	void* XMLCreateSelect(tinyxml2::XMLElement* ele)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//���һ�������ؼ�����
		QSelect* ctrl = (QSelect*)QFUSE::CreateQWND("QFSelect", rect);
		if (!ctrl)return ctrl;


		attr = ele->FirstAttribute();
		//��Ŀ
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("Item"); child != nullptr; child = child->NextSiblingElement("Item")) {
				const tinyxml2::XMLAttribute* value = child->FindAttribute("value");
				if (value) {
					ctrl->InsertItem(stringToWString(std::string(value->Value())).c_str());
					const tinyxml2::XMLAttribute* fcolor1 = child->FindAttribute("fcolor1");
					const tinyxml2::XMLAttribute* fcolorD = child->FindAttribute("fcolorD");
					const tinyxml2::XMLAttribute* fcolor2 = child->FindAttribute("fcolor2");
					const tinyxml2::XMLAttribute* colorD = child->FindAttribute("colorD");
					const tinyxml2::XMLAttribute* color1 = child->FindAttribute("color1");
					const tinyxml2::XMLAttribute* color2 = child->FindAttribute("color2");
					unsigned char d = std::strtoul(colorD->Value(), nullptr, 10);
					int para = 10;
					if ('x' == std::tolower(color1->Value()[1]))para = 16;
					unsigned int c1 = std::strtoul(color1->Value(), nullptr, para);
					para = 10;
					if ('x' == std::tolower(color2->Value()[1]))para = 16;
					unsigned int c2 = std::strtoul(color2->Value(), nullptr, para);
					ctrl->SetItemBKColor(-1,  c1, c2, d);

					d = std::strtoul(fcolorD->Value(), nullptr, 10);
					para = 10;
					if ('x' == std::tolower(fcolor1->Value()[1]))para = 16;
					c1 = std::strtoul(fcolor1->Value(), nullptr, para);
					para = 10;
					if ('x' == std::tolower(fcolor2->Value()[1]))para = 16;
					c2 = std::strtoul(fcolor2->Value(), nullptr, para);

					ctrl->SetItemFontColor(-1,  c1, c2, d);

				}
			}

		}
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {

				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("fontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
			else if ("CurrentItem" == na) {
				int v = std::strtol(attr->Value(), nullptr, 10);
				ctrl->SetCurrentItem(v);
			}
			else if ("FontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);;
				ctrl->SetFontColor(color);
			}
			else if ("ItemFontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetItemFontSize(v);
			}
			else if ("ItemHeight" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetItemHeight(v);
			}
			else if ("PopDialogHeight" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetPopDialogHeight(v);
			}
			else if ("PopDialogWidth" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetPopDialogWidth(v);
			}
			else if ("SelectChangeEvent" == na) {
				//���ת̬�䶯
				ctrl->SetSelectChangeEvent((QSELECT_SELECTCHANGE)GetEventFun(attr->Value()));
			}
			attr = attr->Next();
		}

		return ctrl;//���ذ�ťָ��
	}
	
	void* AnalUI(tinyxml2::XMLElement* ele)
	{
		std::string na = ele->Name();
		if ("QFButton" == na) {
			return XMLCreateButton(ele);

		}
		else if ("QFPButton" == na) {
			return XMLCreateButton2(ele);

		}
		else if ("QFEdit" == na) {
			return XMLCreateEdit(ele);

		}
		else if ("QFReportForm" == na) {
			return XMLCreateReportForm(ele);
		}
		else if ("QFPictureBox" == na) {

			return XMLCreatePictureBox(ele);
		}
		else if ("QFProgressBarCar" == na) {

			return XMLCreateProgressBarCar(ele);
		}
		else if ("QFLineChart" == na) {

			return XMLCreateLineChart(ele);
		}
		else if ("QFTabs" == na) {

			return XMLCreateTable(ele);
		}
		else if ("QFCATLBarChart" == na) {

			return XMLCreateCATLBarChart(ele);
		}
		else if ("QFCATLTempeChart" == na) {

			return XMLCreateCATLTempeChart(ele);
		}
		else if ("QFDrawer" == na) {

			return XMLCreateDrawer(ele);
		}
		else if ("QFHLayout" == na) {

			return XMLCreateHLayout(ele);
		}
		else if ("QFVLayout" == na) {

			return XMLCreateVLayout(ele);
		}
		else if ("QFTLayout" == na) {

			return XMLCreateTLayout(ele);
		}
		else if ("QFStaticText" == na) {

			return XMLCreateStaticText(ele);
		}
		else if ("QFSlider" == na) {

			return XMLCreateSlider(ele);
		}
		else if ("QFSwitch" == na) {

			return XMLCreateSwitch(ele);
		}
		else if ("QFProgress" == na) {

			return XMLCreateProgress(ele);
		}
		else if ("QFColorBox" == na) {

			return XMLCreateColorBox(ele);
		}
		else if ("QFSelect" == na) {

			return XMLCreateSelect(ele);
		}
		return nullptr;
	}
	void ExplainXML(const char* xmlText)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLError result = xmlDoc.Parse(xmlText);
		if (result != tinyxml2::XML_SUCCESS) {
			DebugOut("����QFUI��XMLʧ�ܣ���ȷ�ϴ����XML�����Ƿ���ȷ��");
			return ;
		}
		// ��ȡ��Ԫ��
		tinyxml2::XMLElement* root = xmlDoc.RootElement();
		if (root == nullptr) {
			DebugOut("����QFUI��XMLʧ�ܣ��Ҳ������ڵ� ��QWMange��");
			return ;
		}
		//�ж�root��name�Ƿ�ΪQWMange
		std::string Rname = root->Name();
		if ("QWMange" != Rname) {
			DebugOut("����QFUI��XMLʧ�ܣ����ڵ����ƴ��� \"%s\"", Rname.c_str());
			return ;
		}
		tinyxml2::XMLElement* ele = root->FirstChildElement();
		while (ele) {
			QWND* p = (QWND*)AnalUI(ele);
			if (p) {
				QFUSE::AttachCtrl(p);
			}
			ele = ele->NextSiblingElement();
		}

	}

	void SetPicPathConvertor(PICPATHCONVERTOR fun)
	{
		g_picPathCtor = fun;
	}


};
namespace QFUSE_EW
{
	



	std::wstring stringToWString(const std::string& str) {
		// ����ת������Ļ�������С
		int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
		std::wstring wstr(size_needed, 0);

		// ִ��ת��
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);

		return wstr;
	}
	std::string wstring_to_string(const std::wstring& wstr) {
		if (wstr.empty()) {
			return std::string();
		}

		int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string str(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
		return str;
	}

	std::map<int, CtrlAttribute> g_allCtrl;
	void* AnalUI(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	QRect XML2Rect(const std::string& data);
	QPointF XML2Point(const std::string& data);
	void* XMLCreateButton(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateButton2(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateEdit(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateReportForm(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreatePictureBox(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateProgressBarCar(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateLineChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateTable(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateCATLBarChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateCATLTempeChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateDrawer(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateHLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateVLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateTLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* XMLCreateStaticText(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	void* AnalUI(tinyxml2::XMLElement* ele, DLLImportQWND* parent);
	
	QRect XML2Rect(const std::string& data) {
		QRect rect;
		std::regex pattern(R"(\((\d+),(\d+),(\d+),(\d+)\))");
		std::smatch matches;
		int ci[4];
		if (std::regex_match(data, matches, pattern)) {
			if (matches.size() == 5) { // matches[0] is the whole match, matches[1] to matches[4] are the groups
				for (size_t i = 1; i < matches.size(); ++i) {
					ci[i - 1] = std::stoi(matches[i].str());
				}
			}
		}
		rect.X = ci[0];
		rect.Y = ci[1];
		rect.Width = ci[2];
		rect.Height = ci[3];
		return rect;
	}
	QPointF XML2Point(const std::string& data) {
		QPointF point;
		// ����������ʽģʽ��ƥ��������С����ʽ�������
		std::regex pattern(R"(\(\s*([-+]?\d*\.?\d+)\s*,\s*([-+]?\d*\.?\d+)\s*\))");

		// ����һ��smatch�������ڴ洢ƥ����
		std::smatch match;

		// ���ҵ�һ��ƥ����
		if (std::regex_search(data, match, pattern)) {

			// ��ȡ�����x��y��ֵ
			point.X = std::stod(match[1].str());
			point.Y = std::stod(match[2].str());

		}
		return point;
	}

	void* XMLCreateButton(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());

		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateButton ����hostʧ��");
			return nullptr;
		}
		//���һ��������ť����
		QButton* qct = (QButton*)QFUSE::CreateQWND("QFButton", rect);
		if (!qct) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateButton ����buttonʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(qct);//�Ѱ�ť���޵�host��

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ���ť��������
				qct->SetQWNDName(attr->Value());
		
			}
			else if ("Text" == na) {
				//��Ӹ���ť�����ı�
				qct->SetText(stringToWString(attr->Value()).c_str());
			
			}
			else if ("onClick" == na) {
				//qct->SetClickEvent(QFBUTTONCLICK(GetEventFun(attr->Value())));
				
			}
			else if ("fontName" == na) {
				qct->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				qct->SetFontSize(std::stoi(attr->Value()));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(qct->qwm), { "QFButton",host,parent }));


		//�ѿտǰ�ťȥ��
		qct->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(qct);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete qct;//��������տ�


		return host;//���ذ�ť������ָ��
	}
	void* XMLCreateButton2(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());

		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateButton ����hostʧ��");
			return nullptr;
		}
		//���һ��������ť����
		QButton2* qct = (QButton2*)QFUSE::CreateQWND("QFPButton", rect);
		if (!qct) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateButton ����buttonʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(qct);//�Ѱ�ť���޵�host��

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ���ť��������
				qct->SetQWNDName(attr->Value());

			}
			else if ("Text" == na) {
				//��Ӹ���ť�����ı�
				qct->SetText(stringToWString(attr->Value()).c_str());

			}
			else if ("onClick" == na) {
				//qct->SetClickEvent(QFBUTTONCLICK(GetEventFun(attr->Value())));

			}
			else if ("fontName" == na) {
				qct->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				qct->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				qct->SetFontColor(color);

			}
			else if ("bkColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				qct->SetBKColor(color);

			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(qct->qwm), { "QFPButton",host,parent }));


		//�ѿտǰ�ťȥ��
		qct->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(qct);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete qct;//��������տ�


		return host;//���ذ�ť������ָ��
	}
	void* XMLCreateEdit(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateEdit ����hostʧ��");
			return nullptr;
		}
		//���һ�������༭�����
		QEdit* ctrl = (QEdit*)QFUSE::CreateQWND("QFEdit", rect);
		if (!ctrl){
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateEdit ����editʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��


		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//��Ӹ��༭�������ı�
				ctrl->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}

			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFEdit",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateReportForm(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateReportForm ����hostʧ��");
			return nullptr;
		}
		//���һ�������������
		QReportForm* ctrl = (QReportForm*)QFUSE::CreateQWND("QFReportForm", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateReportForm ����ReportFormʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��




		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("IsDrag" == na) {
				std::string str(attr->Value());
				std::transform(str.begin(), str.end(), str.begin(),
					[](unsigned char c) { return std::tolower(c);
					});
				if ("true" == str) {
					ctrl->IsIsDrag(true);
				}
				else {
					ctrl->IsIsDrag(false);
				}
			}
			else if ("TitleHeight" == na) {
				ctrl->SetTitleHeight(std::stoi(attr->Value()));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		//��������
		tinyxml2::XMLElement* FTitle = ele->FirstChildElement("FTitle");
		if (FTitle) {
			for (tinyxml2::XMLElement* child = FTitle->FirstChildElement("FItem"); child != nullptr; child = child->NextSiblingElement("FItem")) {

				const tinyxml2::XMLAttribute* name = child->FindAttribute("text");
				const tinyxml2::XMLAttribute* width = child->FindAttribute("width");
				if (name && width) {
					//�ÿ�Ⱥ��ı�����һ������
					ctrl->InsertTitle(stringToWString(name->Value()), std::stoi(width->Value()));
				}
			}

		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFReportForm",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreatePictureBox(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreatePictureBox ����hostʧ��");
			return nullptr;
		}
		//���һ������ͼƬ�����
		QPictureBox* ctrl = (QPictureBox*)QFUSE::CreateQWND("QFPictureBox", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreatePictureBox ����QPictureBoxʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("path" == na) {
				//��Ӹ�ͼƬ�����ͼƬ����·������
				std::wstring path = stringToWString(attr->Value());

				ctrl->LoadPicture(path);
			}
			else if ("Transparency" == na) {
				unsigned char tp = 255 * std::stod(attr->Value());
				//ctrl->SetTransparency(tp);
				ctrl->SetTransparency(tp);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFPictureBox",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateProgressBarCar(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateProgressBarCar ����hostʧ��");
			return nullptr;
		}
		//���һ���������ͽ���������
		QProgressBarCar* ctrl = (QProgressBarCar*)QFUSE::CreateQWND("QFProgressBarCar", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateProgressBarCar ����QProgressBarCarʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("posit" == na) {
				//��Ӹ����ͽ��������ý���
				ctrl->SetPercentage(std::stod(attr->Value()));
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFProgressBarCar",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateLineChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateLineChart ����hostʧ��");
			return nullptr;
		}
		//���һ����������ͼ�����
		QLineChart* ctrl = (QLineChart*)QFUSE::CreateQWND("QFLineChart", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateLineChart ����QLineChartʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stoi(attr->Value()));
			}
			else if ("legfontName" == na) {
				ctrl->SetLegFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("legfontSize" == na) {
				ctrl->SetLegFontSize(std::stoi(attr->Value()));
			}
			else if ("XAxisStrHeight" == na) {
				ctrl->SetXAxisStrHeight(std::stod(attr->Value()));
			}
			else if ("YAxisStrWidth" == na) {
				ctrl->SetYAxisStrWidth(std::stod(attr->Value()));
			}
			else if ("XAxisScaleWidth" == na) {
				ctrl->SetXAxisScaleWidth(std::stod(attr->Value()));
			}
			else if ("YAxisScaleWidth" == na) {
				ctrl->SetYAxisScaleWidth(std::stod(attr->Value()));
			}
			else if ("LineWidth" == na) {
				ctrl->SetLineWidth(std::stod(attr->Value()));
			}
			else if ("XAxisRange" == na) {
				QPointF point = XML2Point(attr->Value());
				ctrl->SetXAxisRange(point.X, point.Y);
			}
			else if ("YAxisRange" == na) {
				QPointF point = XML2Point(attr->Value());
				ctrl->SetYAxisRange(point.X, point.Y);
			}
			else if ("LegWidth" == na) {
				ctrl->SetLegWidth(std::stod(attr->Value()));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFLineChart",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateTable(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateTable ����hostʧ��");
			return nullptr;
		}
		//���һ��������ҳ��ǩ����
		QTabs* ctrl = (QTabs*)QFUSE::CreateQWND("QFTabs", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateTable ����QTabsʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		//��ǩ����
		tinyxml2::XMLElement* Tables = ele->FirstChildElement("Tables");
		if (Tables) {
			int index = 0;
			for (tinyxml2::XMLElement* child = Tables->FirstChildElement("Table"); child != nullptr; child = child->NextSiblingElement("Table"), ++index) {

				const tinyxml2::XMLAttribute* text = child->FindAttribute("text");
				if (text) {
					//����һ����ǩ
					//�Ƿ���ͼ��
					const tinyxml2::XMLAttribute* ico = child->FindAttribute("ico");
					if (ico) {
						//����һ����ǩ(��ͼ���)
						std::wstring path = stringToWString(ico->Value());
						ctrl->AddTab(stringToWString(text->Value()).c_str(), path.c_str());
					}
					else {
						ctrl->AddTab(stringToWString(text->Value()).c_str());
					}
					//�鿴��û�и��ӿؼ�
					for (tinyxml2::XMLElement* childCtrl = child->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
						//���ش���һ���ؼ�
						DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl,(DLLImportQWND*)ctrl->qwm);
						//����ؼ�ָ�벻Ϊ�գ�����뵱ǰ��ǩ
						if (p) {
							ctrl->AttachCtrl(p, index);
						}

					}

				}





			}

		}

		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFTabs",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateCATLBarChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateCATLBarChart ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QCATLBarChart* ctrl = (QCATLBarChart*)QFUSE::CreateQWND("QFCATLBarChart", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateCATLBarChart ����QFCATLBarChartʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("ScaleColor" == na) {
				ctrl->SetScaleColor(std::stoi(attr->Value()));
			}
			else if ("ScaleMax" == na) {
				ctrl->SetScaleMax(std::stof(attr->Value()));
			}
			else if ("ScaleMin" == na) {
				ctrl->SetScaleMin(std::stof(attr->Value()));
			}
			else if ("ScaleWidth" == na) {
				ctrl->SetScaleWidth(std::stof(attr->Value()));
			}
			else if ("ScaleHeight" == na) {
				ctrl->SetScaleHeight(std::stof(attr->Value()));
			}
			else if ("BarChartWidth" == na) {
				ctrl->SetBarChartWidth(std::stof(attr->Value()));
			}
			else if ("GridWidth" == na) {
				ctrl->SetGridWidth(std::stof(attr->Value()));
			}
			else if ("LineWidth" == na) {
				ctrl->SetLineWidth(std::stof(attr->Value()));
			}
			else if ("YAxisStrWidth" == na) {
				ctrl->SetYAxisStrWidth(std::stof(attr->Value()));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}

		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateCATLTempeChart(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateCATLTempeChart ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QCATLTempeChart* ctrl = (QCATLTempeChart*)QFUSE::CreateQWND("QFCATLTempeChart", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateCATLTempeChart ����QFCATLTempeChartʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("UnitWidth" == na) {
				ctrl->SetUnitWidth(std::stod(attr->Value()));
			}
			else if ("UnitFontSize" == na) {
				ctrl->SetUnitFontSize(std::stod(attr->Value()));
			}
			else if ("UnitIntervalWidth" == na) {
				ctrl->SetUnitIntervalWidth(std::stod(attr->Value()));
			}
			else if ("AlignWidth" == na) {
				ctrl->SetAlignWidth(std::stod(attr->Value()));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFCATLTempeChart",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateDrawer(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateDrawer ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QDrawer* ctrl = (QDrawer*)QFUSE::CreateQWND("QFDrawer", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateDrawer ����QFDrawerʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("onSelected" == na) {
				//ctrl->SetSelectEvent(QDRAWERSELECT(GetEventFun(attr->Value())));
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
	
		}
		//��Ŀ����
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			int index = 0;
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("Category"); child != nullptr; child = child->NextSiblingElement("Category"), ++index) {
				ctrl->InsertDrawer(-1);
				int id = ctrl->GetDrawerCount() - 1;
				const tinyxml2::XMLAttribute* name = child->FindAttribute("name");
				if (name) {
					std::string v(name->Value());
					ctrl->SetDrawerName(id, stringToWString(v).c_str());
				}
				for (tinyxml2::XMLElement* Subkey = child->FirstChildElement("Subkey"); Subkey != nullptr; Subkey = Subkey->NextSiblingElement("Subkey")) {

					const tinyxml2::XMLAttribute* nm = Subkey->FindAttribute("name");
					if (nm) {
						std::string text = nm->Value();

						ctrl->InsertSubObject(id, -1);
						int sid = ctrl->GetSubObjectCount(id) - 1;
						ctrl->SetSubObjectName(id, sid, stringToWString(text).c_str());

					}



				}

			}
		}



		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFDrawer",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateHLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateProgressBarCar ����hostʧ��");
			return nullptr;
		}
		//���һ������ˮƽ����������
		QHLayout* ctrl = (QHLayout*)QFUSE::CreateQWND("QFHLayout", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateHLayout ����QFHLayoutʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��



		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl,(DLLImportQWND*)ctrl->qwm);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {
				unsigned int weight = 0;
				auto cattri = childCtrl->FindAttribute("weight");
				if (cattri) {
					weight = std::strtoul(cattri->Value(), nullptr, 10);
				}
				ctrl->InsertCtrl(p, weight);

			}

		}

		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFHLayout",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateVLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateProgressBarCar ����hostʧ��");
			return nullptr;
		}
		//���һ��������ֱ����������
		QVLayout* ctrl = (QVLayout*)QFUSE::CreateQWND("QFVLayout", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateVLayout ����QFVLayoutʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��
		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());

			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl, (DLLImportQWND*)ctrl->qwm);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {
				unsigned int weight = 0;
				auto cattri = childCtrl->FindAttribute("weight");
				if (cattri) {
					weight = std::strtoul(cattri->Value(), nullptr, 10);
				}
				ctrl->InsertCtrl(p, weight);
			}


		}

		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFVLayout",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateTLayout(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateTLayout ����hostʧ��");
			return nullptr;
		}
		//���һ��������ֱ����������
		QTLayout* ctrl = (QTLayout*)QFUSE::CreateQWND("QFTLayout", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateTLayout ����QFTLayoutʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);//�Ѱ�ť���޵�host��
		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}

		//�鿴��û�и��ӿؼ�
		for (tinyxml2::XMLElement* childCtrl = ele->FirstChildElement(); childCtrl != nullptr; childCtrl = childCtrl->NextSiblingElement()) {
			//���ش���һ���ؼ�
			DLLImportQWND* p = (DLLImportQWND*)AnalUI(childCtrl, (DLLImportQWND*)ctrl->qwm);
			//����ؼ�ָ�벻Ϊ�գ������һ���ؼ������������
			if (p) {


				ctrl->SetCtrl(p->qwm);
			}


		}

		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFTLayout",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateStaticText(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateStaticText ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QStaticText* ctrl = (QStaticText*)QFUSE::CreateQWND("QFStaticText", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateStaticText ����QFStaticTextʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Text" == na) {
				//���þ�̬�ı����ı�
				ctrl->SetText(stringToWString(attr->Value()).c_str());
			}
			else if ("fontName" == na) {
				ctrl->SetFontName(stringToWString(attr->Value()).c_str());
			}
			else if ("fontSize" == na) {
				ctrl->SetFontSize(std::stod(attr->Value()));
			}
			else if ("fontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			else if ("backgroundColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetBKColor(color);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFStaticText",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateSlider(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateSlider ����hostʧ��");
			return nullptr;
		}
		
		//���һ�������ؼ�����
		QSlider* ctrl = (QSlider*)QFUSE::CreateQWND("QFSlider", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateSlider ����QSliderʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("Direction" == na) {
				//���þ�̬�ı����ı�
				ctrl->SetDirection(std::strtol(attr->Value(),nullptr,10));
			}
			else if ("IsSegmentation" == na) {
				if ("true" == value) {
					ctrl->IsSegmentation(true);
				}
				else {
					ctrl->IsSegmentation(false);
				}
		
			}
			else if ("RailColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);

				ctrl->SetRailColor(color);
			}
			else if ("RailRatio" == na) {
				double rat = std::strtod(attr->Value(),nullptr);
				ctrl->SetRailRatio(rat);
			}
			else if ("backgroundColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetColor(color);
			}
			else if ("maxValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetMaxValue(v);
			}
			else if ("minValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetMaxValue(v);
			}
			else if ("currentValue" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetMaxValue(v);
			}
			else if ("ValueChangeEvent" == na) {
				//�����ֵ�ı亯���ص�����
			
			}

			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		//�ֶ�ֵ
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("SegmValue"); child != nullptr; child = child->NextSiblingElement("SegmValue")) {

				const tinyxml2::XMLAttribute* value = child->FindAttribute("value");
			
				if (value) {
					//����һ���ֶ�
					ctrl->AddSegmVuale(std::strtod(value->Value(),nullptr));
				}
			}

		}




		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFSlider",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateSwitch(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateSwitch ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QSwitch* ctrl = (QSwitch*)QFUSE::CreateQWND("QFSwitch", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateSwitch ����QFSwitchʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();

		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("IsMutex" == na) {
				if ("true") {
					ctrl->IsMutex(true);
				}
				else {
					ctrl->IsMutex(false);
				}
				
			}
			else if ("State" == na) {
				if ("true") {
					ctrl->SetState(true);
				}
				else {
					ctrl->SetState(false);
				}
			}
			else if ("StateChangeEvent" == na) {
				//���ת̬�䶯
			}

			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFSwitch",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateProgress(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateProgress ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QProgress* ctrl = (QProgress*)QFUSE::CreateQWND("QFProgress", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateProgress ����QFProgressʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();
		unsigned int c1 = 0,c2=0;
		int rcCount = 0;
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("CurrentValue" == na) {
				double v = std::strtod(attr->Value(),nullptr);
				ctrl->SetCurrentValue(v);
			}
			else if ("DecimalPlaces" == na) {
				
				unsigned char v = std::strtoul(attr->Value(), nullptr,10);
				ctrl->SetDecimalPlaces(v);
			}
			else if ("Direction" == na) {
				unsigned char v = std::strtoul(attr->Value(), nullptr, 10);
				ctrl->SetDirection(v);
			}
			else if ("FontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetFontColor(color);
			}
			else if ("LineTypeColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);
				ctrl->SetLineTypeProgressColor(color);
			}
			else if ("RingTypeColor1" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				c1 = std::strtoul(attr->Value(), nullptr, para);
				++rcCount;
				if(rcCount>1)
				ctrl->SetRingProgressColor(c1, c2);
			}
			else if ("RingTypeColor2" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				c2 = std::strtoul(attr->Value(), nullptr, para);
				++rcCount;
				if (rcCount > 1)
					ctrl->SetRingProgressColor(c1, c2);
			}
			else if ("FontName" == na) {
				
				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("FontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
			else if ("RingProgressThickness" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetRingProgressThickness(v);
			}
			else if ("Type" == na) {
				unsigned int v = std::strtoul(attr->Value(), nullptr,10);
				ctrl->SetStyle(v);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFProgress",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateColorBox(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateColorBox ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QColorBox* ctrl = (QColorBox*)QFUSE::CreateQWND("QFColorBox", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateColorBox ����QFColorBoxʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();
		unsigned int c1 = 0, c2 = 0;
		int rcCount = 0;
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {

				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("fontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
	
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFColorBox",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	void* XMLCreateSelect(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{
		const tinyxml2::XMLAttribute* attr = ele->FindAttribute("Rect");
		if (!attr) {
			return nullptr;
		}
		std::string xml(attr->Value());
		QRect rect = XML2Rect(xml);
		//�ȴ���QHost
		QHost* host = (QHost*)QFUSE::CreateQWND("QFHostCtrl", rect);
		if (!host) {
			DebugOut("QFUSE_EW::XMLCreateColorBox ����hostʧ��");
			return nullptr;
		}
		//���һ�������ؼ�����
		QSelect* ctrl = (QSelect*)QFUSE::CreateQWND("QFSelect", rect);
		if (!ctrl) {
			QWNDHANDLE::QWNDs.erase(host);//�ӿؼ�����ժ������ؼ��ļ�¼
			delete host;
			DebugOut("QFUSE_EW::XMLCreateColorBox ����QFSelectʧ��");
			return nullptr;
		}
		host->SetParasiticCtrl(ctrl);

		attr = ele->FirstAttribute();
		unsigned int c1 = 0, c2 = 0;
		int rcCount = 0;
		//��Ŀ
		tinyxml2::XMLElement* Items = ele->FirstChildElement("Items");
		if (Items) {
			for (tinyxml2::XMLElement* child = Items->FirstChildElement("Item"); child != nullptr; child = child->NextSiblingElement("Item")) {
				const tinyxml2::XMLAttribute* value = child->FindAttribute("value");
				if (value) {
					ctrl->InsertItem(stringToWString(std::string(value->Value())).c_str());
					const tinyxml2::XMLAttribute* fcolor1 = child->FindAttribute("fcolor1");
					const tinyxml2::XMLAttribute* fcolorD = child->FindAttribute("fcolorD");
					const tinyxml2::XMLAttribute* fcolor2 = child->FindAttribute("fcolor2");
					const tinyxml2::XMLAttribute* colorD = child->FindAttribute("colorD");
					const tinyxml2::XMLAttribute* color1 = child->FindAttribute("color1");
					const tinyxml2::XMLAttribute* color2 = child->FindAttribute("color2");
					unsigned char d = std::strtoul(colorD->Value(), nullptr, 10);
					int para = 10;
					if ('x' == std::tolower(color1->Value()[1]))para = 16;
					unsigned int c1 = std::strtoul(color1->Value(), nullptr, para);
					para = 10;
					if ('x' == std::tolower(color2->Value()[1]))para = 16;
					unsigned int c2 = std::strtoul(color2->Value(), nullptr, para);
					ctrl->SetItemBKColor(-1, c1, c2, d);

					d = std::strtoul(fcolorD->Value(), nullptr, 10);
					para = 10;
					if ('x' == std::tolower(fcolor1->Value()[1]))para = 16;
					c1 = std::strtoul(fcolor1->Value(), nullptr, para);
					para = 10;
					if ('x' == std::tolower(fcolor2->Value()[1]))para = 16;
					c2 = std::strtoul(fcolor2->Value(), nullptr, para);

					ctrl->SetItemFontColor(-1, c1, c2, d);

				}
			}

		}
		while (attr) {
			std::string na = attr->Name();
			std::string value = attr->Value();
			if ("name" == na) {
				//��Ӹ��ؼ���������
				ctrl->SetQWNDName(attr->Value());
			}
			else if ("fontName" == na) {

				ctrl->SetFontName(stringToWString(value).c_str());
			}
			else if ("fontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetFontSize(v);
			}
			else if ("CurrentItem" == na) {
				int v = std::strtol(attr->Value(), nullptr,10);
				ctrl->SetCurrentItem(v);
			}
			else if ("FontColor" == na) {
				int para = 10;
				if ('x' == std::tolower(attr->Value()[1]))para = 16;
				unsigned long color = std::strtoul(attr->Value(), nullptr, para);;
				ctrl->SetCurrentItem(color);
			}
			else if ("ItemFontSize" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetItemFontSize(v);
			}
			else if ("ItemHeight" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetItemHeight(v);
			}
			else if ("PopDialogHeight" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetPopDialogHeight(v);
			}
			else if ("PopDialogWidth" == na) {
				double v = std::strtod(attr->Value(), nullptr);
				ctrl->SetPopDialogWidth(v);
			}
			host->TransferMessage(QM_HOSTCTRL_INSERT_PAIR, DWORD(na.c_str()), DWORD(attr->Value()), 0);//�����Ƽ�¼��host��
			attr = attr->Next();
		}
		



		g_allCtrl.insert(std::pair<int, CtrlAttribute>(int(ctrl->qwm), { "QFSelect",host,parent }));
		//�ѿտǿؼ�ȥ��
		ctrl->qwm = nullptr;
		QWNDHANDLE::QWNDs.erase(ctrl);//�ӿؼ�����ժ������ؼ��ļ�¼
		delete ctrl;//��������տ�
		return host;//���ذ�ťָ��
	}
	
	void* AnalUI(tinyxml2::XMLElement* ele, DLLImportQWND* parent)
	{

		std::string na = ele->Name();
		if ("QFButton" == na) {
			return XMLCreateButton(ele, parent);

		}
		else if ("QFPButton" == na) {
			return XMLCreateButton2(ele, parent);

		}
		else if ("QFEdit" == na) {
			return XMLCreateEdit(ele, parent);

		}
		else if ("QFReportForm" == na) {
			return XMLCreateReportForm(ele, parent);
		}
		else if ("QFPictureBox" == na) {

			return XMLCreatePictureBox(ele, parent);
		}
		else if ("QFProgressBarCar" == na) {

			return XMLCreateProgressBarCar(ele, parent);
		}
		else if ("QFLineChart" == na) {

			return XMLCreateLineChart(ele, parent);
		}
		else if ("QFTabs" == na) {

			return XMLCreateTable(ele, parent);
		}
		else if ("QFCATLBarChart" == na) {

			return XMLCreateCATLBarChart(ele, parent);
		}
		else if ("QFCATLTempeChart" == na) {

			return XMLCreateCATLTempeChart(ele, parent);
		}
		else if ("QFDrawer" == na) {

			return XMLCreateDrawer(ele, parent);
		}
		else if ("QFHLayout" == na) {

			return XMLCreateHLayout(ele, parent);
		}
		else if ("QFVLayout" == na) {

			return XMLCreateVLayout(ele, parent);
		}
		else if ("QFTLayout" == na) {

			return XMLCreateTLayout(ele, parent);
		}
		else if ("QFStaticText" == na) {

			return XMLCreateStaticText(ele, parent);
		}
		else if ("QFSlider" == na) {

			return XMLCreateSlider(ele, parent);
		}
		else if ("QFSwitch" == na) {

			return XMLCreateSwitch(ele, parent);
		}
		else if ("QFProgress" == na) {

			return XMLCreateProgress(ele, parent);
		}
		else if ("QFColorBox" == na) {

			return XMLCreateColorBox(ele, parent);
		}
		else if ("QFSelect" == na) {

			return XMLCreateSelect(ele, parent);
		}
		return nullptr;
	}
	void ExplainXML(const char* xmlText, DLLImportQWND* parent)
	{
		tinyxml2::XMLDocument xmlDoc;
		tinyxml2::XMLError result = xmlDoc.Parse(xmlText);
		if (result != tinyxml2::XML_SUCCESS) {
			DebugOut("����QFUI��XMLʧ�ܣ���ȷ�ϴ����XML�����Ƿ���ȷ��");
			return;
		}
		// ��ȡ��Ԫ��
		tinyxml2::XMLElement* root = xmlDoc.RootElement();
		if (root == nullptr) {
			DebugOut("����QFUI��XMLʧ�ܣ��Ҳ������ڵ� ��QWMange��");
			return;
		}
		//�ж�root��name�Ƿ�ΪQWMange
		std::string Rname = root->Name();
		
		const tinyxml2::XMLAttribute* attr = root->FindAttribute("Rect");
		std::string xml(attr->Value());
		QRect rt = XML2Rect(xml);
		parent->Reconstruct(nullptr, rt);
	
		if ("QWMange" != Rname) {
			DebugOut("����QFUI��XMLʧ�ܣ����ڵ����ƴ��� \"%s\"", Rname.c_str());
			return;
		}
		tinyxml2::XMLElement* ele = root->FirstChildElement();
		//DLLImportQWND* parent = (DLLImportQWND*)QFUSE::GetQWManger();
		while (ele) {
			QWND* p = (QWND*)AnalUI(ele, (DLLImportQWND*) parent->qwm);

			if (p) {
				QEditorWin* qew = (QEditorWin*)parent;
				qew->AttachCtrl((DLLImportQWND*)p);
			}
			ele = ele->NextSiblingElement();
		}

	}
	

};