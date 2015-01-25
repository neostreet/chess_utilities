use chess

drop table if exists blitz_games;

create table blitz_games (
  id mediumint auto_increment primary key,
  blitz_game_date date not null,
  time_control varchar(10) not null,
  opponent_name varchar(25) not null,
  color varchar(1) not null,
  num_half_moves smallint not null,
  result varchar(1) not null,
  result_detail varchar(50) not null,
  opponent_elo_before int null,
  opponent_elo_after int not null,
  my_elo_after int not null,
  opening varchar(10) null
);

quit
