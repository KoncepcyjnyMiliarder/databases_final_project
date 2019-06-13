--utworzenie tabeli liderow i czlonkow
CREATE TABLE member(
  id numeric PRIMARY KEY,
  password_hash text NOT NULL,
  last_activity numeric NOT NULL
);

--utworzenie tabeli pamietajacej, kto sposord czlonkow jest liderem
CREATE TABLE leader(
  id numeric UNIQUE REFERENCES member(id)
);

--tablica projektow
CREATE TABLE project(
  project_id numeric PRIMARY KEY,  
  authority numeric NOT NULL
);

--utworzenie tabeli wsparcia i protestow
CREATE TABLE action(
  action_id numeric PRIMARY KEY,
  member_id numeric REFERENCES member(id),
  project numeric REFERENCES project(project_id),
  action_time numeric NOT NULL,
  is_support boolean NOT NULL,
  upvotes numeric DEFAULT 0,
  downvotes numeric DEFAULT 0
);

--utworzenie tabeli up/downvote
CREATE TABLE vote(
  voter_id numeric REFERENCES member(id),
  action_id numeric REFERENCES action(action_id),
  voting_time numeric NOT NULL,
  is_upvote boolean NOT NULL
);

--tworzenie uzytkownika app i nadanie mu praw
CREATE USER app WITH ENCRYPTED PASSWORD 'qwerty';

--kto jest liderem moze tylko czytac
GRANT SELECT ON leader TO app;

--update timestamp
GRANT SELECT, INSERT, UPDATE ON member TO app;

--update counters
GRANT SELECT, INSERT, UPDATE ON action TO app;

--pozostale tabele jedynie czytac i dopisywac krotki
GRANT SELECT, INSERT ON project TO app;
GRANT SELECT, INSERT ON vote TO app;

--pomocnicze funkcje

--$1 - member id
--$2 - password
--$3 - timestamp
CREATE OR REPLACE FUNCTION make_leader(numeric, text, numeric) RETURNS VOID AS
$X$
	INSERT INTO member(id, password_hash, last_activity) VALUES ($1, crypt($2, gen_salt('bf')), $3);
	INSERT INTO leader(id) VALUES ($1);
$X$
LANGUAGE SQL;

--$1 - member id
--$2 - timestamp
CREATE OR REPLACE FUNCTION is_member_active(numeric, numeric) RETURNS boolean AS
$X$
DECLARE
	final_res boolean;
BEGIN
	SELECT (last_activity + 31556926 > $2) INTO final_res FROM member WHERE id = $1;
	RETURN final_res;
END
$X$
LANGUAGE plpgsql;
