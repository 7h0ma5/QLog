CREATE TABLE IF NOT EXISTS schema_versions (
        version INTEGER PRIMARY KEY,
        updated TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY,
        start_time TEXT,
        end_time TEXT,
        callsign TEXT NOT NULL,
        rst_sent TEXT,
        rst_rcvd TEXT,
        freq REAL,
        band TEXT,
        mode TEXT,
        submode TEXT,
        name TEXT,
        qth TEXT,
        gridsquare TEXY,
        dxcc INTEGER,
        country TEXT,
        cont TEXT,
        cqz INTEGER,
        ituz INTEGER,
        pfx TEXT,
        state TEXT,
        cnty TEXT,
        iota TEXT,
        qsl_rcvd CHECK(qsl_rcvd IN ('N', 'Y', 'R', 'I')) NOT NULL DEFAULT 'N',
        qsl_rdate TEXT,
        qsl_sent CHECK(qsl_sent IN ('N', 'Y', 'R', 'Q', 'I')) NOT NULL DEFAULT 'N',
        qsl_sdate TEXT,
        lotw_qsl_rcvd CHECK(qsl_rcvd IN ('N', 'Y', 'R', 'I')) NOT NULL DEFAULT 'N',
        lotw_qslrdate TEXT,
        lotw_qsl_sent CHECK(qsl_sent IN ('N', 'Y', 'R', 'Q', 'I')) NOT NULL DEFAULT 'N',
        lotw_qslsdate TEXT,
        tx_pwr REAL,
        fields JSON
);


CREATE TABLE IF NOT EXISTS bands (
        id INTEGER PRIMARY KEY,
        name TEXT UNIQUE NOT NULL,
        start_freq FLOAT,
        end_freq FLOAT,
        enabled BOOLEAN
);

CREATE TABLE IF NOT EXISTS modes (
        id INTEGER PRIMARY KEY,
        name TEXT UNIQUE NOT NULL,
        submodes JSON,
        rprt TEXT,
        dxcc TEXT CHECK(dxcc IN ('CW', 'PHONE', 'DIGITAL')) NOT NULL,
        enabled INTEGER
);

CREATE TABLE IF NOT EXISTS dxcc_entities (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        prefix TEXT,
        cont TEXT,
        cqz INTEGER,
        ituz INTEGER,
        lat REAL,
        lon REAL,
        tz REAL
);

CREATE TABLE IF NOT EXISTS dxcc_prefixes (
        id INTEGER PRIMARY KEY,
        prefix TEXT UNIQUE NOT NULL,
        exact INTEGER,
        dxcc INTEGER REFERENCES dxcc_entities(id),
        cqz INTEGER,
        ituz INTEGER,
        cont TEXT,
        lat REAL,
        lon REAL
);

CREATE INDEX prefix_idx ON dxcc_prefixes(prefix);
CREATE INDEX callsign_idx ON contacts(callsign);
