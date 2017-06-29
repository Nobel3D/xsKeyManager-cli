#include <xslib/xslib.h>
#include <libxspasswd/xspasswd.h>

#define ISACTIVE if(!lib->tableActive()) { xsConsole() << "None table is selected!" << endl; return; }
#define CHECK_INPUT(expr) { if(expr) { xsConsole() << "Too few arguments in " << __func__ << endl; return; } }

QString strCmd;
QStringList args;
xsPasswd* lib = nullptr;

QStringList params(const QString &str) //TODO: Move into Shell method in xslib
{
    QStringList offset = str.split(' ');
    for(int i = 0; i < offset.size(); i++)
    {
        offset.at(i).trimmed();
        if(offset.at(i).isEmpty())
            offset.removeAt(i--);
    }
    return offset;
}

void create(const QStringList &in)
{
    CHECK_INPUT(in.size() < 2);
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
    CHECK_INPUT(in.size() < 2);
    if(!lib->tableUse(in.at(1)))
        xsConsole() << "Impossible to find table " << in.at(1) << endl;
    else
        xsConsole() << "Using table " << in.at(1) << endl;
}

void add(const QStringList &in)
{
    ISACTIVE;
    if(in.count() == 1)
    {
        QStringList tableFields = lib->tableField();
        QStringList fields;
        QString buffer;
        QStringList values;
        for(int i = 0; i < tableFields.count(); i++)
        {
            xsConsole() << "Insert into field name \"" << tableFields.at(i) << "\" ->";
            xsConsole() >> buffer;
            if(buffer != "")
            {
                values.append(buffer);
                fields.append(tableFields.at(i));
            }
        }
        lib->dataAdd(fields, values);
    }
    else
    {
        QStringList out = in;
        if(!lib->dataAdd(out))
            xsConsole() << lib->strStatus;
    }
}
void get(const QStringList &in)
{
    ISACTIVE;
    CHECK_INPUT(in.size() < 2);
    QStringList out;
    bool ok;
    int row = in.at(1).toInt(&ok);
    switch (in.size()) {
    case 2:
        row = in.at(1).toInt(&ok);
        if(!ok)
            out = lib->dataGet(in.at(1));
        else
            out = lib->dataGet(row);
        break;
    case 3:
        out = lib->dataGet(in.at(1), in.at(2));
        break;
    default:
        xsConsole() << "Malformed parameters!" << endl;
        return;
    }
    for(int i = 0; i < out.size(); i++)
        if(!out.at(i).isEmpty())
            xsConsole() << out.at(i) << endl;
}

void update(const QStringList &in)
{
    ISACTIVE;
    CHECK_INPUT(in.size() < 4);
    bool isInt = false;
    int value = in.at(3).toInt(&isInt);
    if(isInt)
    {
        if(!lib->dataUpdate(in.at(1), in.at(2), value))
            xsConsole() << "Value doesn't exist!" << endl;
    }
    else
    {
        if(!lib->dataUpdate(in.at(1), in.at(2), in.at(3)))
            xsConsole() << "Value doesn't exist!" << endl;
    }
}

void remove(const QStringList &in)
{
    ISACTIVE;
    CHECK_INPUT(in.size() < 2);
    bool isInt = false;
    int index = in.at(1).toInt(&isInt);
    if(isInt)
    {
        if(!lib->dataDelete(index))
            xsConsole() << "Index doesn't exist!" << endl;
    }
    else
        xsConsole() << "Input index must be a number!" << endl;
}

void gen(const QStringList &in)
{
    CHECK_INPUT(in.size() < 2);
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

void field()
{
    ISACTIVE;
    QStringList out = lib->tableField();
    for(int i = 0; i < out.count(); i++)
        xsConsole() << out.at(i) << endl;
}

void import(const QStringList &in)
{
    CHECK_INPUT(in.size() < 4);
    if(in.at(1).compare("table", Qt::CaseInsensitive) == 0)
        lib->importTable(in.at(2), in.at(3));
    else if(in.at(1).compare("database", Qt::CaseInsensitive) == 0)
        lib->importDatabase(in.at(2), in.at(3));
}

void _export(const QStringList &in)
{
    CHECK_INPUT(in.size() < 3);
    if(in.at(1).compare("table", Qt::CaseInsensitive) == 0)
    {
        ISACTIVE;
        lib->exportTable(in.at(2));
    }
    else if(in.at(1).compare("database", Qt::CaseInsensitive) == 0)
    {
        lib->exportDatabase(in.at(2));
    }
}

void usage()
{
    xsConsole()
           << "gen <length> [setting]" << endl
           << "use <table-name>" << endl
           << "list" << endl
           << "field" << endl
           << "add <value1> <value2> ..." << endl
           << "get <field> <value>" << endl
           << "update <field> <oldvalue> <newvalue>" << endl
           << "remove <index>" << endl;
}

int main(int argc, char *argv[])
{
    lib = new xsPasswd();
    login(PWFILE);

    while(true)
    {
        args = params(xsConsole::Shell(GETUSER, lib->database->getTable()));
        strCmd = args.value(0);

        if(strCmd.compare("use",Qt::CaseInsensitive) == 0)
            use(args);
        else if(strCmd.compare("add",Qt::CaseInsensitive) == 0)
            add(args);
        else if(strCmd.compare("get",Qt::CaseInsensitive) == 0)
            get(args);
        else if(strCmd.compare("update",Qt::CaseInsensitive) == 0)
            update(args);
        else if(strCmd.compare("remove",Qt::CaseInsensitive) == 0)
            remove(args);
        else if(strCmd.compare("create",Qt::CaseInsensitive) == 0)
            create(args);
        else if(strCmd.compare("gen",Qt::CaseInsensitive) == 0)
            gen(args);
        else if(strCmd.compare("list", Qt::CaseInsensitive) == 0)
            list();
        else if(strCmd.compare("field",Qt::CaseInsensitive) == 0)
            field();
        else if(strCmd.compare("import",Qt::CaseInsensitive) == 0)
            import(args);
        else if(strCmd.compare("export",Qt::CaseInsensitive) == 0)
            _export(args);
        else if(strCmd.compare("quit", Qt::CaseInsensitive) == 0)
            break;
        else
            usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
