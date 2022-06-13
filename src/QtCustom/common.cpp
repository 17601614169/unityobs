#include "common.h"

#include <obs.h>
#include <QCoreApplication>

QString common::GetSourceIcon(const char *id)
{
	QString str;

	obs_icon_type type = obs_source_get_icon_type(id);

	switch (type) {
	case OBS_ICON_TYPE_IMAGE:
		str = ":/res/images/tupian_icon.png";
		break;
	case OBS_ICON_TYPE_DESKTOP_CAPTURE:
		str = ":/res/images/pinmu_icon.png";
		break;
	case OBS_ICON_TYPE_WINDOW_CAPTURE:
		str = ":/res/images/chuangkou_icon.png";
		break;
	case OBS_ICON_TYPE_CAMERA:
		str = ":/res/images/shexiang_icon.png";
		break;
	case OBS_ICON_TYPE_TEXT:
		str = ":/res/images/wenzi_icon.png";
		break;
	case OBS_ICON_TYPE_MEDIA:
		str = ":/res/images/wenzi_icon.png";
		break;
	default:
		break;
	}

	return str;
}

QString common::GetSourceId(SourceType sourcetype)
{
	QString id;
	switch (sourcetype) {
	case SOURCE_CAMERA:
		break;
	case SOURCE_PICTURE:
		id = "image_source";
		break;
	case SOURCE_TEXT:
		id = "text_gdiplus";
		break;
	case SOURCE_WINODW:
		break;
	case SOURCE_SCREEN:
		break;
	case SOURCE_MEDIA:
		break;
	default:
		break;
	}
	return id;

	//int array[6] = {14, 1, 8, 12, 11, 6};

	//const char *unversioned_type;
	//const char *type;

	//if (obs_enum_input_types2(array[int(sourcetype)], &type,
	//			  &unversioned_type))
	//	return QString(unversioned_type);

	//return nullptr;
}

const char *common::GetSourceDisplayNameFromId(const char *id)
{
	// 暂时屏蔽
	/*if (strcmp(id, "scene") == 0)
		return Str("Basic.Scene");*/
	if (strcmp(id, "scene") == 0)
		return nullptr;
	const char *v_id = obs_get_latest_input_type_id(id);
	return obs_source_get_display_name(v_id);
}


QPixmap common::GetPixmap(OBSPixmap pix) {
	QPixmap pixmap;
	switch (pix) {
	case PIXMAP_TRAY_ICON: {
		pixmap.load(":/res/images/keepfun.png");
	}break;
	}
	return pixmap;
}

long long common::color_to_int(QColor color)
{
	auto shift = [&](unsigned val, int shift) {
			return ((val & 0xff) << shift);
	};

	return shift(color.red(), 0) | shift(color.green(), 8) |
	       shift(color.blue(), 16) | shift(color.alpha(), 24);
}

QColor common::color_from_int(long long val)
{
	return QColor(val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff,
		      (val >> 24) & 0xff);
}

QString common::GetFilterPath(Filter_Type type)
{
	QString progpath = QCoreApplication::instance()->applicationDirPath();
	QString path("");

	switch (type) {
	case Filter_Normal:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/original.png";
		break;
	case Filter_Baixi:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_baixi.png";
		break;
	case Filter_Chaobai:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_chaobai.png";
		break;
	case Filter_Qingyang:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_qingyang.png";
		break;
	case Filter_Baixue:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_baixue.png";
		break;
	case Filter_Chulian:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_chulian.png";
		break;
	case Filter_Naiyou:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_naiyou.png";
		break;
	case Filter_Qingchun:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_qingchun.png";
		break;
	case Filter_Rouhe:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_rouhe.png";
		break;
	case Filter_Weixun:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_weixun.png";
		break;
	case Filter_Chuxin:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_chuxin.png";
		break;
	case Filter_Feifan:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_feifan.png";
		break;
	case Filter_Huopo:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_huopo.png";
		break;
	case Filter_Musi:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_musi.png";
		break;
	case Filter_Qiangwei:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_qiangwei.png";
		break;
	case Filter_Dongren:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_dongren.png";
		break;
	case Filter_Landiao:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_landiao.png";
		break;
	case Filter_Naihui:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_naihui.png";
		break;
	case Filter_Quqi:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_quqi.png";
		break;
	case Filter_Rixi:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_rixi.png";
		break;
	case Filter_Shensui:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_shensui.png";
		break;
	case Filter_Fu:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_fugu.png";
		break;
	case Filter_Bai:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_lihuabai.png";
		break;
	case Filter_Fen:
		path = progpath + "/../../data/obs-plugins/obs-filters/LUTs/filter_rouwufen.png";
		break;
	default:
		break;
	}
	return path;
}
