BEGIN TRANSACTION;
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
	`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
	`username`	TEXT NOT NULL UNIQUE,
	`password`	TEXT NOT NULL DEFAULT "INVALID"
);
DROP TABLE IF EXISTS `todos`;
CREATE TABLE `todos` (
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
DROP TABLE IF EXISTS `history`;
CREATE TABLE `history` (
	`todo`	INTEGER,
	`type`	TEXT,
	`content`	TEXT,
	`timestamp`	INTEGER
);
DROP TABLE IF EXISTS `blockedby`;
CREATE TABLE `blockedby` (
	`blocked`	INTEGER NOT NULL,
	`blocking`	INTEGER NOT NULL,
	PRIMARY KEY(blocked,blocking)
) WITHOUT ROWID;
COMMIT;
