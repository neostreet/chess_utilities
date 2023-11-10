use chess

drop table if exists blitz_games;

create table blitz_games (
  id mediumint auto_increment primary key,
  game_filename varchar(30) not null,
  game_date date not null,
  time_control varchar(10) not null,
  opponent_name varchar(25) not null,
  color varchar(1) not null,
  num_moves smallint not null,
  my_checks smallint not null,
  opponent_checks smallint not null,
  result varchar(1) not null,
  mate varchar(1) not null,
  result_detail varchar(25) not null,
  my_elo_before int not null,
  my_elo_delta smallint not null,
  opponent_elo_before int not null,
  opponent_elo_delta smallint not null,
  eco varchar(4) not null,
  opening varchar(150) not null
);

quit
