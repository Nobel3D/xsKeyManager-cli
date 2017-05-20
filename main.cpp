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
    if(!lib->tableUse(in.value(1)))
        xsConsole() << "Impossible to find table " << in.value(1) << endl;
    else
        xsConsole() << "Using table " << in.value(1) << endl;
}

void add(const QStringList &in)
{
    QStringList out = in;
    if(!lib->dataAdd(out))
        xsConsole() << lib->strStatus;
}
void get(const QStringList &in)
{
    QStringList out = lib->dataGet(in);
    for(int i = 0; i < out.size(); i++)
        xsConsole() << out.at(i) << endl;
}

void update(const QStringList &in)
{
    if(!lib->dataUpdate(in))
        xsConsole() << "Value doesn't exist!" << endl << lib->strStatus << endl;
}

void gen(const QStringList &in)
{
    xsConsole() << lib->generatePassword(in) << endl;
}

void login(const QString &filepw)
{
    if(lib->loadPassword(filepw))
    {
        do {
            xsConsole() << "(" << QString::number(lib->password->getHit()) << "/" << QString::number(lib->password->getMaxHit()) <<  ") Enter your password -> ";
        } while(!lib->userJoin(xsConsole::ReadPasswd(true)));
        xsConsole() << "Welcome " << GETUSER << endl;
    }
    else
    {
        xsConsole() << "Enter new password -> ";
        xsPassword hot = xsConsole::ReadPasswd();
        xsConsole() << "Repeat your password -> ";
        if(!hot.Check(xsConsole::ReadPasswd()))
        {
            xsConsole() << "Password mismatch!\n";
            exit(0);
        }

        if(!lib->userCreate(hot, filepw))
            xsConsole() << lib->strStatus;

        xsConsole() << "Creation successful on your home directory!\n";
        login(filepw);
    }
}

void list()
{
    QStringList out = lib->tableList();
    for(int i = 0; i < out.count(); i++)
        xsConsole() << out.at(i) << endl;
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
        if(strCmd.compare("update",Qt::CaseInsensitive) == 0)
            update(args);
        if(strCmd.compare("create",Qt::CaseInsensitive) == 0)
            create(args);
        if(strCmd.compare("gen",Qt::CaseInsensitive) == 0)
            gen(args);
        if(strCmd.compare("list", Qt::CaseInsensitive) == 0)
            list();
        //xsConsole() << usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
