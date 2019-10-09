INSERT INTO modes (name, rprt, dxcc, enabled, submodes) VALUES
('CW', '599', 'CW', true, NULL),
('SSB', '59', 'PHONE', true, '["LSB", "USB"]'),
('AM', '59', 'PHONE', true, NULL),
('FM', '59', 'PHONE', true, NULL),
('PSK', '599', 'DIGITAL', true, '[
        "PSK31",
        "PSK63",
        "PSK63F",
        "PSK125",
        "PSK250",
        "PSK500",
        "PSK1000",
        "QPSK31",
        "QPSK63",
        "QPSK125",
        "QPSK250",
        "QPSK500"
]'),
('RTTY', '599', 'DIGITAL', true, NULL),
('MFSK', '599', 'DIGITAL', true, '[
        "FT4",
        "MFSK4",
        "MFSK8",
        "MFSK11",
        "MFSK16",
        "MFSK22",
        "MFSK31",
        "MFSK32"
]'),
('OLIVIA', NULL, 'DIGITAL', true, '[
        "OLIVIA 4/125",
        "OLIVIA 4/250",
        "OLIVIA 8/250",
        "OLIVIA 8/500",
        "OLIVIA 16/500",
        "OLIVIA 16/1000",
        "OLIVIA 32/1000"
]'),
('JT65', '-1', 'DIGITAL', true, '[
        "JT65A",
        "JT65B",
        "JT65B2",
        "JT65C",
        "JT65C2"
]'),
('JT9', '-1', 'DIGITAL', true, '[
        "JT9-1",
        "JT9-2",
        "JT9-5",
        "JT9-10",
        "JT9-30"
]'),
('FT8', '-1', 'DIGITAL', true, NULL),
('HELL', NULL, 'DIGITAL', true, '[
        "FMHELL",
        "FSKHELL",
        "HELL80",
        "HFSK",
        "PSKHELL"
]'),
('CONTESTIA', NULL, 'DIGITAL', true, NULL),
('DOMINO', NULL, 'DIGITAL', true, '[
        "DOMINOEX",
        "DOMINOF"
]'),
('MT63', NULL, 'DIGITAL', true, NULL),
('JT6M', '26', 'DIGITAL', true, NULL),
('JTMSK', '-1', 'DIGITAL', true, NULL),
('MSK144', '0', 'DIGITAL', true, NULL),
('FSK441', '26', 'DIGITAL', true, NULL),
('DIGITALVOICE', '59', 'PHONE', true, NULL),
('DSTAR', '59', 'PHONE', true, NULL),
('PKT', NULL, 'DIGITAL', true, NULL),
('ATV', NULL, 'DIGITAL', true, NULL),
('SSTV', NULL, 'DIGITAL', true, NULL);
