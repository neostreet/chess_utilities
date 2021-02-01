use chess

drop table if exists rapid_games;

create table rapid_games (
  id mediumint auto_increment primary key,
  game_date date not null,
  time_control varchar(10) not null,
  opponent_name varchar(25) not null,
  color varchar(1) not null,
  num_moves smallint not null,
  result varchar(1) not null,
  result_decimal decimal(6,1) null,
  result_detail varchar(25) not null,
  opponent_elo_before int null,
  opponent_elo_after int null,
  opponent_elo_delta smallint null,
  my_elo_before int null,
  my_elo_after int null,
  my_elo_delta smallint null,
  eco varchar(3) null
);

quit
