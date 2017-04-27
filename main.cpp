#include <xslib/xslib.h>
#include <QFile>
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>

using namespace std;



int main(int argc, char *argv[])
{
    while(buffer != "quit")
    {
        xsConsole() << status();
        xsConsole() >> buffer;

        if(buffer.startsWith("create ",Qt::CaseInsensitive) && joined == 0)
            if(create(buffer.split(" ").value(1)) == FAIL)
                return 0;
        if(buffer.startsWith("join ",Qt::CaseInsensitive) && joined == 0)
            if(join(buffer.split(" ").value(1)) == FAIL)
                return 0;
        if(buffer.startsWith("use ",Qt::CaseInsensitive) && joined == 1)
            use(buffer);
        if(buffer.startsWith("add ",Qt::CaseInsensitive) && joined == 1)
            add(buffer);
        if(buffer.startsWith("get ",Qt::CaseInsensitive) && joined == 1)
            get(buffer);
        //xsConsole() << usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
