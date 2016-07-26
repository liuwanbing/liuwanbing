//
//  GGRecomGameCell.m
//  ThePeopleTV
//
//  Created by aoyolo on 16/3/30.
//  Copyright © 2016年 高广. All rights reserved.
//

#import "GGMovieCell.h"
#import "ggmovieModel.h"
#import "AFNetworking.h"
@implementation GGMovieCell

- (void)awakeFromNib {
}
-(void)setCellModel:(GGMovieModel *)obj{
    
    self.backgroundColor = [UIColor whiteColor];
    
    self.title.text = obj.title;
    self.nick.text = obj.nick;
    
    self.avatar.layer.cornerRadius = self.avatar.frame.size.width/2;
    self.avatar.layer.masksToBounds = YES;
    self.thumb.layer.cornerRadius = 8;
    self.thumb.layer.masksToBounds = YES;
    
    [self.thumb sd_setImageWithURL:[NSURL URLWithString:obj.thumb]];
    if ([obj.avatar isEqualToString:@""]) {
        self.avatar.image = [UIImage imageNamed:@"nil"];
    }else{
        [self.avatar sd_setImageWithURL:[NSURL URLWithString:obj.avatar]];
    }
    
    
    
}
@end
