#include <xslib/xslib.h>
#include <libxspasswd/xspasswd.h>

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

void create(const QStringList &in)
{
    QString table;

    if(in.size() == 2)
        table = in.value(1);
    else
    {
        xsConsole() << "New table name ->";
        xsConsole() >> table;
    }

    QString field;
    QStringList fieldlist;

    while(true)
    {
        xsConsole() << "New field name (just enter to stop) ->";
        xsConsole() >> field;
        if(field == "")
            break;
        fieldlist.append(field.trimmed());
    }
    lib->tableCreate(table, fieldlist);
}

void use(const QStringList &in)
{
    if(lib->tableUse(in.value(1)) == FAIL)
        xsConsole() << "Impossible to find table " << in.value(1) << endl;
    else
        xsConsole() << "Using table " << in.value(1) << endl;
}

void add(const QStringList &in)
{
    QStringList out = in;
    lib->dataAdd(out);
    xsConsole() << lib->strStatus;
}
void get(const QStringList &in)
{
    QStringList out = lib->dataGet(in);
    for(int i = 0; i < out.size(); i++)
        xsConsole() << out.at(i) << endl;
}

void login(const QString &filepw)
{
    if(lib->loadPassword(filepw) == OK)
    {
        do {
            if(lib->password->getMaxHit() < lib->password->getHit())
                exit(0);
            xsConsole() << "(" << QString::number(lib->password->getMaxHit()) << "/" << QString::number(lib->password->getHit()) <<  ") Enter your password -> ";
        } while(lib->userJoin(xsConsole::ReadPasswd(true)) == FAIL);
        xsConsole() << "Welcome " << GETUSER << "\n";
    }
    else
    {
        xsConsole() << "Enter new password -> ";
        xsPassword hot = xsConsole::ReadPasswd();
        xsConsole() << "Repeat your password -> ";
        if(hot.Check(xsConsole::ReadPasswd()) == FAIL)
        {
            xsConsole() << "Password mismatch!\n";
            exit(0);
        }

        if(lib->userCreate(hot, filepw) == FAIL)
            xsConsole() << lib->strStatus;

        xsConsole() << "Creation successful on your home directory!\n";
        login(filepw);
    }
}

int main(int argc, char *argv[])
{
    lib = new xsPasswd();
    login(PWFILE);

    while(!strCmd.startsWith("quit", Qt::CaseInsensitive))
    {
        args = params(xsConsole::Shell(GETUSER, lib->database->getTable()));
        strCmd = args.value(0);

        if(strCmd.compare("use",Qt::CaseInsensitive) == 0)
            use(args);
        if(strCmd.compare("add",Qt::CaseInsensitive) == 0)
            add(args);
        if(strCmd.compare("get",Qt::CaseInsensitive) == 0)
            get(args);
        if(strCmd.compare("create",Qt::CaseInsensitive) == 0)
            create(args);
        //xsConsole() << usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
