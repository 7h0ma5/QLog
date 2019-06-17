#ifndef MIGRATION_H
#define MIGRATION_H

class Migration {
public:
    Migration() {}

    bool run();

private:
    bool migrate(int toVersion);
    int getVersion();
    bool setVersion(int version);

    bool migrate1();

    static const int latestVersion = 1;
};

#endif // MIGRATION_H
