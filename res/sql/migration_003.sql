CREATE TABLE IF NOT EXISTS dxcc_entities (
        id SERIAL PRIMARY KEY,
        name VARCHAR(50) NOT NULL,
        prefix VARCHAR(10),
        cont VARCHAR(2),
        cqz INTEGER,
        ituz INTEGER,
        lat FLOAT,
        lon FLOAT,
        tz FLOAT
);

CREATE TABLE IF NOT EXISTS dxcc_prefixes (
        id SERIAL PRIMARY KEY,
        prefix VARCHAR(25) UNIQUE NOT NULL,
        exact BOOLEAN,
        dxcc INTEGER REFERENCES dxcc_entities(id),
        cqz INTEGER,
        ituz INTEGER,
        cont VARCHAR(2),
        lat FLOAT,
        lon FLOAT
);
