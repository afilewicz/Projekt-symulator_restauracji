#pragma once

using table_id = uint;

class Table
{
    private:
        table_id id;
    public:
        Table(table_id id);
        table_id get_id() const;
};
