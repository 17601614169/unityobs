#include "common-function.h"

namespace CommonFunction {
	QPixmap GetMidPixmap(const QString& file, int w) {
		QPixmap pixmap;
		bool bLoad = pixmap.load(file);
		if (bLoad) {
			int total = pixmap.width();
			if (total > w) {
				QPixmap pix;
				pix = pixmap.copy((total-w)/2,0,w,pixmap.height());
				pixmap.detach();
				pixmap = pix;
			}
		}
		return pixmap;
	}
};
