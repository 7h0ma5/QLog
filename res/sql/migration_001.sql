CREATE TABLE IF NOT EXISTS schema_versions (
        version SERIAL PRIMARY KEY,
        updated TIMESTAMP NOT NULL
);

CREATE TABLE IF NOT EXISTS contacts (
        id SERIAL PRIMARY KEY,
        start_time TIMESTAMP WITH TIME ZONE,
        end_time TIMESTAMP WITH TIME ZONE,
        callsign VARCHAR(25) NOT NULL,
        rst_sent VARCHAR(10),
        rst_rcvd VARCHAR(10),
        freq DECIMAL(12,6),
        band VARCHAR(10),
        mode VARCHAR(15),
        submode VARCHAR(15),
        name VARCHAR(50),
        qth VARCHAR(50),
        gridsquare VARCHAR(12),
        dxcc INTEGER,
        country VARCHAR(50),
        cont VARCHAR(2),
        cqz INTEGER,
        ituz INTEGER,
        pfx VARCHAR(10),
        fields JSON
);
