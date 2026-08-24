BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "sample_libraries" (
	"id"	INTEGER NOT NULL UNIQUE,
	"display_name"	TEXT NOT NULL,
	"path"	TEXT NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "sound_libraries" (
	"id"	INTEGER NOT NULL UNIQUE,
	"display_name"	TEXT NOT NULL,
	"description"	TEXT,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "sound_library_samples" (
	"f_sound_library_id"	INTEGER NOT NULL,
	"f_sample_library_id"	INTEGER NOT NULL,
	FOREIGN KEY("f_sample_library_id") REFERENCES "sample_libraries" ON DELETE CASCADE,
	PRIMARY KEY("f_sound_library_id","f_sample_library_id"),
	FOREIGN KEY("f_sound_library_id") REFERENCES "sound_libraries" ON DELETE CASCADE
);
COMMIT;
