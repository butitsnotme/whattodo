BEGIN TRANSACTION;
CREATE TABLE `users` (
	`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	`username`	TEXT NOT NULL UNIQUE,
	`salt`	TEXT NOT NULL UNIQUE,
	`password`	TEXT NOT NULL
);
CREATE TABLE "todos" (
	`user`	INTEGER NOT NULL,
	`title`	TEXT,
	`description`	TEXT,
	`due`	INTEGER,
	`parent`	INTEGER,
	`worked`	INTEGER DEFAULT 0,
	`estimate`	INTEGER DEFAULT 0,
	`percent`	INTEGER DEFAULT 0,
	`priority`	INTEGER,
	`status`	TEXT
);
CREATE TABLE `history` (
	`todo`	INTEGER,
	`type`	TEXT,
	`content`	TEXT,
	`timestamp`	INTEGER
);
CREATE TABLE `blockedby` (
	`blocked`	INTEGER NOT NULL,
	`blocking`	INTEGER NOT NULL,
	PRIMARY KEY(blocked,blocking)
) WITHOUT ROWID;
COMMIT;
