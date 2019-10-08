CREATE TYPE qsl_rcvd_status AS ENUM (
        'N',
        'Y',
        'R',
        'I'
);

CREATE TYPE qsl_sent_status AS ENUM (
        'N',
        'Y',
        'R',
        'Q',
        'I'
);

ALTER TABLE contacts
        ADD COLUMN state VARCHAR(50),
        ADD COLUMN cnty VARCHAR(50),
        ADD COLUMN iota VARCHAR(10),
        ADD COLUMN qsl_rcvd qsl_rcvd_status NOT NULL DEFAULT 'N',
        ADD COLUMN qsl_rdate DATE,
        ADD COLUMN qsl_sent qsl_sent_status NOT NULL DEFAULT 'N',
        ADD COLUMN qsl_sdate DATE,
        ADD COLUMN lotw_qsl_rcvd qsl_rcvd_status NOT NULL DEFAULT 'N',
        ADD COLUMN lotw_qslrdate DATE,
        ADD COLUMN lotw_qsl_sent qsl_sent_status NOT NULL DEFAULT 'N',
        ADD COLUMN lotw_qslsdate DATE,
        ADD COLUMN tx_pwr FLOAT;

UPDATE contacts SET state = fields ->> 'state', fields = fields - 'state';
UPDATE contacts SET cnty = fields ->> 'cnty', fields = fields - 'cnty';
UPDATE contacts SET iota = fields ->> 'iota', fields = fields - 'iota';
UPDATE contacts SET tx_pwr = cast(fields ->> 'tx_pwr' as FLOAT), fields = fields - 'tx_pwr';

UPDATE contacts SET qsl_rcvd = cast(fields ->> 'qsl_rcvd' as qsl_rcvd_status), fields = fields - 'qsl_rcvd'
        WHERE (fields ->> 'qsl_rcvd')::name = ANY(enum_range(null::qsl_rcvd_status)::name[]);

UPDATE contacts SET qsl_sent = cast(fields ->> 'qsl_sent' as qsl_sent_status), fields = fields - 'qsl_sent'
        WHERE (fields ->> 'qsl_sent')::name = ANY(enum_range(null::qsl_sent_status)::name[]);

UPDATE contacts SET lotw_qsl_rcvd = cast(fields ->> 'lotw_qsl_rcvd' as qsl_rcvd_status), fields = fields - 'lotw_qsl_rcvd'
        WHERE (fields ->> 'lotw_qsl_rcvd')::name = ANY(enum_range(null::qsl_rcvd_status)::name[]);

UPDATE contacts SET lotw_qsl_sent = cast(fields ->> 'lotw_qsl_sent' as qsl_sent_status), fields = fields - 'lotw_qsl_sent'
        WHERE (fields ->> 'lotw_qsl_sent')::name = ANY(enum_range(null::qsl_rcvd_status)::name[]);
