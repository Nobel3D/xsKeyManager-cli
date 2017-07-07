#include <xslib/xslib.h>
#include <libStronghold/stronghold.h>

#define ISACTIVE if(pem->tableActive().isEmpty()) { xsConsole() << "None table is selected!" << endl; return; }
#define CHECK_INPUT(expr) { if(expr) { xsConsole() << "Too few arguments in " << __func__ << endl; return; } }

QString strCmd;
QStringList args;
PEM* pem = nullptr;
SUM* sum = nullptr;


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
    pem->tableCreate(table, fieldlist);
}

void use(const QStringList &in)
{
    CHECK_INPUT(in.size() < 2);
    if(!pem->tableUse(in.at(1)))
        xsConsole() << "Impossible to find table " << in.at(1) << endl;
    else
        xsConsole() << "Using table " << in.at(1) << endl;
}

void add(const QStringList &in)
{
    ISACTIVE;
    if(in.count() == 1)
    {
        QStringList tableFields = pem->tableField();
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
        pem->add(fields, values);
    }
    else
    {
        QStringList out = in;
        if(!pem->add(out))
            xsConsole() << "Impossible to add data!";
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
            out = pem->get(in.at(1));
        else
            out = pem->get(row);
        break;
    case 3:
        out = pem->get(in.at(1), in.at(2));
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
        if(!pem->update(in.at(1), in.at(2), value))
            xsConsole() << "Value doesn't exist!" << endl;
    }
    else
    {
        if(!pem->update(in.at(1), in.at(2), in.at(3)))
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
        if(!pem->remove(index))
            xsConsole() << "Index doesn't exist!" << endl;
    }
    else
        xsConsole() << "Input index must be a number!" << endl;
}

void gen(const QStringList &in)
{
    CHECK_INPUT(in.size() < 2);
    xsConsole() << pem->generatePassword(in) << endl;
}

void login()
{
    QString name;
    while(true)
    {
        xsConsole() << "Enter your user name -> ";
        xsConsole() >> name;
        xsConsole() << "Enter your user password -> ";
        if(sum->login(name, xsConsole::ReadPasswd(true).getClearPassword()))
        {
            pem = new PEM(sum->getUser());
            break;
        }
    }
    xsConsole() << "Welcome " << name << endl;
}

void list()
{
    QStringList out = pem->tableList();
    for(int i = 0; i < out.count(); i++)
        xsConsole() << out.at(i) << endl;
}

void field()
{
    ISACTIVE;
    QStringList out = pem->tableField();
    for(int i = 0; i < out.count(); i++)
        xsConsole() << out.at(i) << endl;
}

void import(const QStringList &in)
{
    if(in.at(1).compare("table", Qt::CaseInsensitive) == 0)
    {
        CHECK_INPUT(in.size() < 4);
        pem->importTable(in.at(2), in.at(3));
    }
    else if(in.at(1).compare("database", Qt::CaseInsensitive) == 0)
    {
        CHECK_INPUT(in.size() < 3);
        QFileInfoList list;
        for(int i = 0; i < in.size() - 2; i++)
            list.append(QFileInfo(in.at(i + 2)));
        pem->importDatabase(list);
    }
}

void _export(const QStringList &in)
{
    CHECK_INPUT(in.size() < 3);
    if(in.at(1).compare("table", Qt::CaseInsensitive) == 0)
    {
        ISACTIVE;
        pem->exportTable(in.at(2));
    }
    else if(in.at(1).compare("database", Qt::CaseInsensitive) == 0)
    {
        pem->exportDatabase(in.at(2));
    }
}

void admin(const QStringList &in)
{
    CHECK_INPUT(in.size() < 3);
    if(in.at(1).compare("add", Qt::CaseInsensitive) == 0)
    {
        QString name, database;
        while(true)
        {
            xsConsole() << "Enter new user name ->";
            xsConsole() >> name;
            xsConsole() << "Enter new password -> ";
            xsPassword hot = xsConsole::ReadPasswd();
            xsConsole() << "Repeat your password -> ";
            if(!hot.Check(xsConsole::ReadPasswd()))
                xsConsole() << "Password mismatch!\n";
            else
            {
                xsConsole() << "Enter new database path -> ";
                xsConsole() >> database;
                //if(sum->add(name, hot, database))
                   break;
            }
        }
        xsConsole() << "Creation successful on " << database << endl;
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
           << "get [field <value>/<index>]" << endl
           << "update <field> <oldvalue> <newvalue>" << endl
           << "remove <index>" << endl
           << "export [table/database] <path>" << endl
           << "import [table <newname>/database] <path>" << endl;
}

int main(int argc, char *argv[])
{
    sum = new SUM(LOGINFILE);
    login();

    while(true)
    {
        args = xsConsole::Shell(sum->getUser()->getName(), pem->tableActive());
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
        else if(strCmd.compare("admin", Qt::CaseInsensitive) == 0)
            admin(args);
        else if(strCmd.compare("quit", Qt::CaseInsensitive) == 0)
            break;
        else
            usage();

    }
    xsConsole() << "BYE!\n";

    return 0;
}
