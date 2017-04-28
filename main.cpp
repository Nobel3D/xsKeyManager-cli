#include <xslib/xslib.h>
#include <libxspasswd/xspasswd.h>
#include <QDebug>

using namespace std;

QString strBuffer;
QString strCmd;
QStringList args;
xsPasswd* lib = nullptr;

QStringList params(const QString &str)
{
    QStringList offset = str.split(' ');
    for(int i = 0; i < offset.size(); i++)
        offset.value(i).trimmed();
    return offset;
}

void create()
{
    QString hot1,hot2;
    xsConsole() << "Enter new password -> ";
    hot1 = xsConsole::ReadPasswd();
    xsConsole() << "Repeat your password -> ";
    hot2 = xsConsole::ReadPasswd();
    if(hot1 != hot2)
    {
        xsConsole() << "Password mismatch!\n";
        return;
    }
    hot2.clear();

    if(lib->userCreate(hot1) == FAIL)
    {
        xsConsole() << lib->strStatus;
    }
    else
    {
        xsConsole() << "Creation successful on your home directory!\n";
    }
}

void join()
{
    QString passwd;
    do {
    if(lib->iHit > lib->iMaxHit)
        return;
    xsConsole() << "Enter new password (" << QString::number(lib->iHit) << "/" << QString::number(lib->iMaxHit) << ") -> ";
    passwd = xsConsole::ReadPasswd();
    } while(lib->userJoin(passwd) == FAIL);
    xsConsole() << "Welcome " << GETUSER << "\n";
}

void use(const QStringList &in)
{
    if(lib->tableUse(in.value(1)) == FAIL)
        xsConsole() << "Impossible to find table " << in.value(1);
    else
        xsConsole() << "Using table " << in.value(1);
}

void add(const QStringList &in)
{
    QStringList out = in;
    lib->dataAdd(out);
}
void get(const QStringList &in)
{
    lib->dataGet(in);
}

int main(int argc, char *argv[])
{
    lib = new xsPasswd();
    join();

    while(!strBuffer.startsWith("quit", Qt::CaseInsensitive))
    {
        xsConsole() << "[" << GETUSER << "]$ ";
        xsConsole() >> strBuffer;
        args = params(strBuffer);
        strCmd = args.value(0);

        if(strCmd.compare("use",Qt::CaseInsensitive) == 0)
            use(args);
        if(strCmd.compare("add",Qt::CaseInsensitive) == 0)
            add(args);
        if(strCmd.compare("get",Qt::CaseInsensitive) == 0)
            get(args);
        //xsConsole() << usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
